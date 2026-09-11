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

#include "engine.h"
#include "log.h"
#include "physics.h"
#include "scene.h"
#include "timing.h"
#include "types.h"
#include "utils.h"
#include "window_info.h"


#include <raylib.h>
#include "rp3d.h"

#include <expected>
#include <optional>
#include <string>

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


static inline auto game_loop() -> void {
    auto& engine = arcxel::Engine::singleton(std::make_optional(arcxel::Scene(1000)));

    while (engine.is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, engine.sample_record);


        {
            const auto _ = arcxel::Timespan(Label::Events, engine.sample_record);
            engine.handle_events();
        }


        const f64 delta = GetFrameTime();

        {
            const auto _ = arcxel::Timespan(Label::PhysicsUpdate, engine.sample_record);
            arcxel::Physics::singleton().update(delta);
        }


        {
            const auto _ = arcxel::Timespan(Label::Update, engine.sample_record);
            engine.update(delta);
        }


        {
            const auto _ = arcxel::Timespan(Label::Render, engine.sample_record);
            engine.render(delta);
        }
    }
}


[[nodiscard]] static auto run() -> arcxel::Fallible {

    // ---- WINDOW CREATION ----
    const auto winfo =
        arcxel::WindowInfo{.width = WIDTH, .height = HEIGHT, .target_fps = 0};

    if (auto r = create_window(winfo); !r) {
        return r;
    }

    // ---- PHYSICS WORLD CREATION ----
    auto physics_info = rp3d::PhysicsWorld::WorldSettings{};
    physics_info.isSleepingEnabled = false;
    physics_info.gravity = rp3d::Vector3(0, -9.81, 0);
    auto& _ = arcxel::Physics::singleton(std::make_optional(std::move(physics_info)));
    arcxel::log(LogLevel::Info, "physics world created");


    // ---- GAME LOOP ----
    DisableCursor();
    game_loop();
    EnableCursor();

    arcxel::Engine::singleton().stop();

    return {};
}

auto main() -> int {

    // ---- OPEN LOGGING ----
    if constexpr (arcxel::logging_enabled) {
        const auto r = arcxel::create_dir(DEFAULT_LOGS_DIR).and_then([](auto&& path) {
            arcxel::capture_raylib_logs();
            return arcxel::open_log_file(path);
        });

        if (!r) {
            arcxel::raw_log("{}", r.error());
        }

    } else {
        SetTraceLogLevel(LOG_NONE);
    }
    

    // ---- CREATE PROFILE TRACE STORE ----
    if constexpr (arcxel::profiling_enabled) {
        if (const auto r = arcxel::create_dir(DEFAULT_TRACES_DIR); !r) {
            arcxel::raw_log("{}", r.error());
        };
    }


    // ---- ENGINE ----
    if (const auto r = run(); !r) {
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
    }

    return 0;
}
