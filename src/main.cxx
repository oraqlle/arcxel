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

namespace log = arcxel::log;
// clang-format on


constexpr i32 WIDTH = 1920;
constexpr i32 HEIGHT = 1080;


/**
 * @brief Set up logging and profile tracing for engine
 */
[[nodiscard]] static auto initialise_tracing() -> arcxel::Fallible {
    // ---- LOGGING ----
    if constexpr (arcxel::log::logging_enabled) {
        const auto r = arcxel::create_dir("logs").and_then([](auto&& path) {
            arcxel::log::capture_raylib_logs();
            return arcxel::log::open_log_file(path);
        });

        if (!r) {
            return r;
        }

    } else {
        SetTraceLogLevel(LOG_NONE);
    }


    // ---- PROFILING ----
    if constexpr (arcxel::timing::profiling_enabled) {
        const auto r = arcxel::create_dir("profs").and_then([](auto&& path) {
            arcxel::log::capture_raylib_logs();
            // set up profiling
            return arcxel::Fallible{};
            // return arcxel::log::open_log_file(path);
        });

        if (!r) {
            return r;
        }
    }

    // ~4M samples, about ten minutes of uncapped frames
    // Samples past this are dropped, not reallocated
    arcxel::timing::reserve(1U << 22U);
}


/**
 * @brief Clean up logging and profile tracing for engine. Flushes final traces and logs
 * remaining in memory and closes files.
 */
[[nodiscard]] static auto deinitialise_tracing() -> arcxel::Fallible {
    // ---- PROFILING ----
    if constexpr (arcxel::timing::profiling_enabled) {
        arcxel::timing::log_summary();

        if (arcxel::timing::write_run_csv().empty()) {
            return std::unexpected("timing: could not write this run's samples");
        }
    }

    // ---- LOGGING ---- <<< Reverse order as logging should be the last thing to close
    if constexpr (arcxel::log::logging_enabled) {
        if (const auto r = arcxel::log::close_log_file(); !r) {
            return r;
        }
    }

    return {};
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
    arcxel::log::log(
        log::LogLevel::Info, "window opened {}x{}", winfo.width, winfo.height
    );

    return {};
}


static inline auto game_loop() -> void {
    auto engine = arcxel::Engine();
    while (engine.is_running()) {
        const auto frame = arcxel::timing::Span(frame_label);


        {
            const auto span = arcxel::timing::Span(events_label);
            engine.handle_events();
        }


        const f64 delta = GetFrameTime();


        {
            const auto span = arcxel::timing::Span(update_label);
            engine.update(delta);
        }


        {
            const auto span = arcxel::timing::Span(render_label);
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

    // ---- GAME LOOP ----
    DisableCursor();
    game_loop();
    EnableCursor();

    return {};
}

auto main() -> int {

    if (const auto r = initialise_tracing(); !r) {
        log(log::LogLevel::Fatal, "{}", result.error());
    }

    if (const auto result = run(); !result) {
        log(log::LogLevel::Fatal, "{}", result.error());
    }

    if (const auto r = deinitialise_tracing(); !r) {
        log(log::LogLevel::Fatal, "{}", result.error());
    }

    return 0;
}
