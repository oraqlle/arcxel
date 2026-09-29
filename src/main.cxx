// <main.cxx> -*- C++ -*-

//  Arcxel Test Bench
//  Copyright (C) 2026  Tyler Swann, Georgia Kanellis
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License v2.1 as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//  USA

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

#include <charconv>
#include <cstdlib>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

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


// which frame loop to run, set by -a
enum class Architecture : u8 {
    Serial,
    Broad,
    Fine
};


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
    Architecture arch = Architecture::Serial;
    std::optional<usize> num_threads = std::nullopt; // nullopt uses hardware_concurrency()
    arcxel::Workload workload = arcxel::Workload{};
}; // struct Config


/**
 * @brief Parse an integer flag input, logging and returning nullopt on failure
 */
template <typename T>
[[nodiscard]] static auto parse_integer(std::string_view flag, const std::string& arg)
    -> std::optional<T> {
    auto value = T{ 0 };
    auto [ptr, err] = std::from_chars(arg.data(), arg.data() + arg.size(), value);

    if (err != std::errc()) {
        arcxel::log(
            LogLevel::Error,
            "Parsing {} flag input \"{}\" failed with error condition {}, "
            "ignoring argument",
            flag,
            arg,
            std::make_error_condition(err).message());

        return std::nullopt;
    }

    if (ptr != arg.data() + arg.size()) {
        arcxel::log(
            LogLevel::Warning,
            "Partial parsing of {} flag input \"{}\", failed at character "
            "number {}",
            flag,
            arg,
            ptr - arg.data());
    }

    return value;
}


/**
 * @brief Parse a float flag input, logging and returning nullopt on failure
 */
[[nodiscard]] static auto parse_float(std::string_view flag, const std::string& arg)
    -> std::optional<f32> {
    // strtof, libc++ from_chars has no float overload
    char* end = nullptr;
    const auto value = std::strtof(arg.c_str(), &end);

    if (end == arg.c_str()) {
        arcxel::log(
            LogLevel::Error,
            "Parsing {} flag input \"{}\" failed, ignoring argument",
            flag,
            arg);

        return std::nullopt;
    }

    if (*end != '\0') {
        arcxel::log(
            LogLevel::Warning,
            "Partial parsing of {} flag input \"{}\", failed at character "
            "number {}",
            flag,
            arg,
            end - arg.c_str());
    }

    return value;
}


[[nodiscard]] static auto parse_args(const std::vector<std::string> args)
    -> std::expected<Config, std::string> {
    auto config = Config{};

    for (auto idx = 1U; idx < args.size(); idx++) {
        const auto& flag = args[idx];

        if (flag == "-h" || flag == "--help") {
            return std::unexpected(
                arcxel::make_log_string(
                    LogLevel::Info,
                    "Arcxel Sample Game Engine\n"
                    "Usage: program [options]\n"
                    "  -n <count>             Number of simulation objects\n"
                    "  -a, --arch <name>      Frame architecture: serial, broad\n"
                    "  -t, --threads <count>  Worker threads for broad, default: "
                    "hardware concurrency\n"
                    "  -w, --work <iters>     Synthetic work per object, default: 0\n"
                    "  -v, --variance <0..1>  Unevenness of synthetic work, default: 0\n"
                    "  -h, --help             Show this help"));
        }

        const auto is_arch = flag == "-a" || flag == "--arch";
        const auto is_threads = flag == "-t" || flag == "--threads";
        const auto is_work = flag == "-w" || flag == "--work";
        const auto is_variance = flag == "-v" || flag == "--variance";

        if (!(flag == "-n" || is_arch || is_threads || is_work || is_variance)) {
            arcxel::log(LogLevel::Warning, "Unknown flag \"{}\", ignoring argument", flag);
            continue;
        }

        // every remaining flag takes an input
        idx += 1;

        if (idx >= args.size()) {
            arcxel::log(
                LogLevel::Error,
                "Input flag {} specified with no input, ignoring argument",
                flag);

            continue;
        }

        const auto& arg = args[idx];

        if (flag == "-n") {
            if (const auto n = parse_integer<usize>(flag, arg)) {
                config.num_sim_objects = *n;
            }
        } else if (is_arch) {
            if (arg == "serial") {
                config.arch = Architecture::Serial;
            } else if (arg == "broad") {
                config.arch = Architecture::Broad;
            }
            // else if (arg == "fine") {
            //     config.arch = Architecture::Fine;
            // }
            else {
                arcxel::log(
                    LogLevel::Warning,
                    "Unknown {} input \"{}\", using serial",
                    flag,
                    arg);
            }
        } else if (is_threads) {
            if (const auto n = parse_integer<usize>(flag, arg)) {
                if (*n < 1) {
                    arcxel::log(
                        LogLevel::Warning,
                        "{} input \"{}\" must be at least 1, using default",
                        flag,
                        arg);
                } else {
                    config.num_threads = n;
                }
            }
        } else if (is_work) {
            if (const auto n = parse_integer<u32>(flag, arg)) {
                config.workload.magnitude = *n;
            }
        } else if (is_variance) {
            if (const auto v = parse_float(flag, arg)) {
                if (*v < 0.0f || *v > 1.0f) {
                    arcxel::log(
                        LogLevel::Warning,
                        "{} input \"{}\" outside 0..1, using 0",
                        flag,
                        arg);
                } else {
                    config.workload.variance = *v;
                }
            }
        }
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


static inline auto game_loop(Config config) -> void {
    auto& engine = arcxel::Engine::singleton(std::make_optional(
        arcxel::Scene(config.num_sim_objects, arcxel::Scene::DEFAULT_BOX_SIZE, config.workload)));

    // log run settings so each csv can be matched to them
    arcxel::log(
        LogLevel::Info,
        "architecture: {}",
        config.arch == Architecture::Broad ? "broad" : "serial");
    arcxel::log(
        LogLevel::Info,
        "workload: {} iterations, variance {}",
        config.workload.magnitude,
        config.workload.variance);

    if (config.arch == Architecture::Broad) {
        auto& pool = arcxel::BroadThreadPool::singleton(config.num_threads);
        arcxel::log(LogLevel::Info, "thread pool started with {} workers", pool.size());
    }

    while (engine.is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, engine.sample_record);

        const f64 delta = GetFrameTime();

        switch (config.arch) {
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
