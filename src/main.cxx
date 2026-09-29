#include "conf.h"
#include "engine.h"
#include "log.h"
#include "physics.h"
#include "rp3d.h"
#include "scene.h"
#include "timing.h"
#include "types.h"
#include "utils.h"
#include "window_info.h"
#include "workload.h"
#include "serial/frame.h"
#include "broad/frame.h"
#include "broad_thread_pool.h"

#include <raylib.h>

#include <expected>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <cstdlib>
#include <string_view>


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
using Label = arcxel::Sample::Label;
// clang-format on


constexpr std::string_view DEFAULT_LOGS_DIR = "logs";
constexpr std::string_view DEFAULT_TRACES_DIR = "traces";

constexpr i32 WIDTH = 1920;
constexpr i32 HEIGHT = 1080;


[[nodiscard]] static auto make_args(i32 argc, char* argv[]) -> std::vector<std::string> {
    auto args = std::vector<std::string>();
    args.reserve(argc);

    for (auto idx = 0; idx < argc; idx++) {
        args.push_back(argv[idx]);
    }

    return args;
}


struct Config {
    usize num_sim_objects = arcxel::default_num_sim_objects;
}; // struct Config



[[nodiscard]] static auto parse_args(const std::vector<std::string> args)
    -> std::expected<Config, std::string> {
    auto config = Config{};

    for (auto idx = 1U; idx < args.size(); idx++) {
        if (args[idx] == "-h") {
            return std::unexpected(
                arcxel::make_log_string(
                    LogLevel::Info,
                    "Arcxel Sample Game Engine\nUsage: program [options]\n  -n,    "
                    "Number of "
                    "simulation objects\n  -h, --help         Show this help"));
        }

        if (args[idx] == "-n") {
            idx += 1;

            if (idx >= args.size()) {
                arcxel::log(
                    LogLevel::Error,
                    "Input flag -n specified with no input, ignoring argument");

                continue;
            }

            const auto& arg = args[idx];
            auto num_objects = usize{ 0 };
            auto [ptr, err] = std::from_chars(
                arg.data(),
                arg.data() + arg.size(),
                num_objects);

            if (err != std::errc()) {
                arcxel::log(
                    LogLevel::Error,
                    "Parsing -n flag input \"{}\" failed with error condition {}, "
                    "ignoring argument",
                    arg,
                    ptr - arg.data(),
                    std::make_error_condition(err).message());
            } else {
                if (ptr != arg.data() + arg.size()) {
                    arcxel::log(
                        LogLevel::Warning,
                        "Partial parsing of -n flag input \"{}\", failed at character "
                        "number {}",
                        arg,
                        ptr - arg.data());
                }

                config.num_sim_objects = num_objects;
            }
        }
    }

    return config;
}


enum class Architecture : u8 {
    Serial,
    Broad,
    Fine
};


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

[[nodiscard]] static auto architecture_from_env() -> Architecture {
    const auto* value = std::getenv("ARCXEL_ARCH");

    if (value == nullptr) {
        arcxel::log(LogLevel::Warning, "ARCXEL_ARCH not found, using serial");
        return Architecture::Serial;
    }

    const auto name = std::string_view(value);

    if (name == "serial") {
        arcxel::log(LogLevel::Info, "ARCXEL_ARCH == serial");
        return Architecture::Serial;
    }
    else if (name == "broad") {
        arcxel::log(LogLevel::Info, "ARCXEL_ARCH == broad");
        return Architecture::Broad;
    }
    // else if (name == "fine") {
    //     arcxel::log(LogLevel::Info, "ARCXEL_ARCH == fine");
    //     return Architecture::Fine;
    // }

    arcxel::log(LogLevel::Warning, "unknown ARCXEL_ARCH '{}', using serial", name);
    return Architecture::Serial;

}

[[nodiscard]] static auto thread_count_from_env() -> std::optional<usize> {
    const auto* value = std::getenv("ARCXEL_THREADS");

    // use hardware_concurrency()
    if (value == nullptr) {
        return std::nullopt;
    }

    const auto count = std::atoi(value);

    if (count < 1) {
        arcxel::log(LogLevel::Warning, "ARCXEL_THREADS '{}' invalid, using default", value);
        return std::nullopt;
    }

    return std::make_optional(static_cast<usize>(count));
}

[[nodiscard]] static auto workload_from_env() -> arcxel::Workload {
    auto workload = arcxel::Workload{};

    // TODO
    // ARCXEL_WORK -> workload.magnitude
    // getenv, nullptr leaves it 0
    // atoi, warn and keep 0 if negative

    // TODO
    // ARCXEL_WORK_VARIANCE -> workload.variance
    // getenv, nullptr leaves it 0.0
    // atof, warn and keep 0.0 if outside 0..1

    // log both
    // lets each csv be matched to its settings
    arcxel::log(LogLevel::Info, "workload: {} iterations, variance {}",
                workload.magnitude, workload.variance);

    return workload;
}


static inline auto game_loop(Config config) -> void {
    auto& engine = arcxel::Engine::singleton(
        std::make_optional(arcxel::Scene(config.num_sim_objects)));

    const auto arch = architecture_from_env();

    // get num threads
    if (arch == Architecture::Broad) {
        auto& pool = arcxel::BroadThreadPool::singleton(thread_count_from_env());
        arcxel::log(LogLevel::Info, "thread pool started with {} workers", pool.size());
    }

    while (engine.is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, engine.sample_record);
        
        const f64 delta = GetFrameTime();

        switch (arch) {
            case Architecture::Broad:
                  arcxel::broad::run_frame(engine, delta);
                  break;

            case Architecture::Serial:
            default:
                arcxel::serial::run_frame(engine, delta);
                break;
        }
    }
}


[[nodiscard]] static auto run(Config config) -> arcxel::Fallible {

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


    // ---- GAME LOOP ----
    DisableCursor();
    game_loop(config);
    EnableCursor();

    arcxel::Engine::singleton().stop();

    return {};
}

auto main(int argc, char* argv[]) -> int {

    // ---- OPEN LOGGING ----
    if constexpr (arcxel::logging_enabled) {
        const auto r = arcxel::create_dir(DEFAULT_LOGS_DIR).and_then([](auto&& path) {
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

    // ---- ARG PARSING ----
    const auto args = make_args(argc, argv);
    auto config = Config{};

    if (const auto r = parse_args(args); !r) {
        arcxel::raw_log("{}", r.error());
        std::exit(-1);
    } else {
        config = r.value();
    }


    // ---- CREATE PROFILE TRACE STORE ----
    if constexpr (arcxel::profiling_enabled) {
        if (const auto r = arcxel::create_dir(DEFAULT_TRACES_DIR); !r) {
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

        if (const auto r = sample_records.write_timings_to_csv(DEFAULT_TRACES_DIR); !r) {
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
