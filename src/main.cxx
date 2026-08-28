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
#include "timing.h"
#include "types.h"
#include "utils.h"
#include "window_info.h"

#include <raylib.h>

#include <expected>
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
// clang-format on


// ~4M samples, ~10mins of uncapped frames. Further Samples are dropped, no reallocation
constexpr usize MAX_SAMPLES = 1U << 22U;

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


static inline auto game_loop(arcxel::SampleRecord& store) -> void {
    auto engine = arcxel::Engine();
    while (engine.is_running()) {

        const auto span = arcxel::Timespan(arcxel::Sample::Label::Frame, store);


        {
            const auto span = arcxel::Timespan(arcxel::Sample::Label::Events, store);
            engine.handle_events();
        }


        const f64 delta = GetFrameTime();


        {
            const auto span = arcxel::Timespan(arcxel::Sample::Label::Update, store);
            engine.update(delta);
        }


        {
            const auto span = arcxel::Timespan(arcxel::Sample::Label::Render, store);
            engine.render(delta, store);
        }
    }
}


[[nodiscard]] static auto run(arcxel::SampleRecord& store) -> arcxel::Fallible {

    // ---- WINDOW CREATION ----
    const auto winfo =
        arcxel::WindowInfo{.width = WIDTH, .height = HEIGHT, .target_fps = 0};

    if (auto r = create_window(winfo); !r) {
        return r;
    }

    // ---- GAME LOOP ----
    DisableCursor();
    game_loop(store);
    EnableCursor();

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
    auto store = arcxel::SampleRecord(0);
    if constexpr (arcxel::profiling_enabled) {
        store = arcxel::SampleRecord(MAX_SAMPLES);

        if (const auto r = arcxel::create_dir(DEFAULT_TRACES_DIR); !r) {
            arcxel::raw_log("{}", r.error());
        };
    }


    // ---- ENGINE ----
    if (const auto r = run(store); !r) {
        arcxel::raw_log("{}", r.error());
    }


    // ---- WRITE PROFILE TRACE ----
    if constexpr (arcxel::profiling_enabled) {
        arcxel::log_trace_summary(store);

        if (const auto r = store.write_timings_to_csv(DEFAULT_TRACES_DIR); !r) {
            arcxel::raw_log("{}", r.error());
        }
    }


    // ---- CLOSE LOGGING ----
    if (const auto r = arcxel::close_log_file(); !r) {
        arcxel::raw_log("{}", r.error());
    }

    return 0;
}
