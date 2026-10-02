#include "conf.h"
#include "engine.h"
#include "log.h"
#include "physics.h"
#include "rp3d.h"
#include "scene.h"
#include "thread_pool.h"
#include "timing.h"
#include "game_loop.h"
#include "types.h"
#include "utils.h"
#include "window_info.h"
#include "workload.h"

#include <raylib.h>
#include <cxxopts.hpp>

#include <cstdlib>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>


// clang-format off
using arcxel::i8;
using arcxel::i16;
using arcxel::i32;
using arcxel::i64;

using arcxel::u8;
using arcxel::u16;
using arcxel::u32;
using arcxel::u64;

using arcxel::isize;
using arcxel::usize;

using arcxel::f32;
using arcxel::f64;

using arcxel::LogLevel;
// clang-format on


constexpr i32 WIDTH = 1920;
constexpr i32 HEIGHT = 1080;


[[nodiscard]] static auto make_config_from_cli_opts(cxxopts::ParseResult opts) -> arcxel::Config {
    auto config = arcxel::Config{};

    if (opts.count("num_objects")) {
        config.num_sim_objects = opts["num_objects"].as<usize>();
    }

    if (opts.count("trace")) {
        config.trace_dir = opts["trace"].as<std::string>();
    }

    if (opts.count("log")) {
        config.log_dir = opts["log"].as<std::string>();
    }

	return config;
}


/**
 * @brief Create raylib window instance, validating it opened correctly
 */
[[nodiscard]] static auto create_window(const arcxel::WindowInfo& winfo)
    -> arcxel::Fallible {
    InitWindow(winfo.width, winfo.height, winfo.name.c_str());

    if (!IsWindowReady()) {
        CloseWindow();
        return std::unexpected("arcxel: window failed to initialise");
    }

    SetTargetFPS(winfo.target_fps); // 0 leaves the frame rate uncapped
    arcxel::log(LogLevel::Info, "window opened {}x{}", winfo.width, winfo.height);

    return {};
}


[[nodiscard]] static auto run(arcxel::Config config) -> arcxel::Fallible {

    // ---- WINDOW CREATION ----
    const auto winfo = arcxel::WindowInfo{ .width = WIDTH,
                                           .height = HEIGHT,
                                           .target_fps = 0 };

    if (auto r = create_window(winfo); !r) {
        return r;
    }

    // ---- PHYSICS WORLD CREATION ----
    auto phys_info = rp3d::PhysicsWorld::WorldSettings{};
    phys_info.isSleepingEnabled = true;
    phys_info.gravity = rp3d::Vector3(0, -9.81f, 0);

    auto& phys_sys = arcxel::Physics::singleton(std::make_optional(std::move(phys_info)));
    arcxel::log(LogLevel::Info, "physics world created");

    if constexpr (arcxel::physics_debug_renderer_enabled) {
        phys_sys.world->setIsDebugRenderingEnabled(true);
        auto& dbgr = phys_sys.world->getDebugRenderer();

        dbgr.setIsDebugItemDisplayed(
            rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE,
            true);
        dbgr.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
        dbgr.setIsDebugItemDisplayed(
            rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE_NORMAL,
            true);

        arcxel::log(LogLevel::Info, "debug renderer enabled for physics engine");
    } else {
        arcxel::log(LogLevel::Info, "debug renderer disabled for physics engine");
    }

    auto& engine = arcxel::Engine::singleton({ arcxel::Scene(config.num_sim_objects) });

    // ---- GAME LOOP ----
    DisableCursor();
    game_loop(config);
    EnableCursor();

    engine.stop();

    return {};
}

auto main(int argc, char* argv[]) -> int {

    // ---- ARG PARSER ----
    auto cli_options = cxxopts::Options{ "arcxel", "Arcxel Testbed" };
    cli_options.add_options()
        ("n,num_objects", "Number of objects to run simulation with", cxxopts::value<usize>())
        ("t,trace", "Output directory of trace file", cxxopts::value<std::string>())
        ("l,log", "Output directory of log file", cxxopts::value<std::string>())
        ("h,help", "Show help");

    auto parsed_cli_opts = cli_options.parse(argc, argv);
    if (parsed_cli_opts.count("help")) {
        arcxel::raw_log("{}", cli_options.help());
        std::exit(0);
    }

    const auto config = make_config_from_cli_opts(parsed_cli_opts);


    // ---- OPEN LOGGING ----
    if constexpr (arcxel::logging_enabled) {
        const auto r = arcxel::create_dir(config.log_dir).and_then([](auto&& path) {
            arcxel::capture_raylib_logs();
            return arcxel::open_log_file(path);
        });

        if (!r) {
            arcxel::raw_log("{}", r.error());
            std::exit(-1);
        }
    } else {
        SetTraceLogLevel(LOG_NONE);
    }


    // ---- CREATE PROFILE TRACE STORE ----
    if constexpr (arcxel::profiling_enabled) {
        if (const auto r = arcxel::create_dir(config.trace_dir); !r) {
            arcxel::raw_log("{}", r.error());
            std::exit(-1);
        };
    }


    // ---- ENGINE ----
    if (const auto r = run(config); !r) {
        arcxel::raw_log("{}", r.error());
    }


    // ---- WRITE PROFILE TRACE ----
    if constexpr (arcxel::profiling_enabled) {
        arcxel::log_trace_summary(arcxel::Engine::singleton().sample_record);
        auto& sample_records = arcxel::Engine::singleton().sample_record;

        if (const auto r = sample_records.write_timings_to_csv(config.trace_dir); !r) {
            arcxel::raw_log("{}", r.error());
        }
    }


    // ---- CLOSE LOGGING ----
    if (const auto r = arcxel::close_log_file(); !r) {
        arcxel::raw_log("{}", r.error());
        std::exit(-1);
    }

    std::exit(0);
}
