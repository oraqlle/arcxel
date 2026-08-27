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
#include "window.h"
#include "window_info.h"

#include <iostream>
#include <ostream>
#include <raylib.h>

#include <expected>
#include <string>
#include <syncstream>

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

static auto create_window(const arcxel::WindowInfo& winfo) -> arcxel::Fallible {
    InitWindow(winfo.width, winfo.height, winfo.name.c_str());

    if (!IsWindowReady()) {
        CloseWindow();
        return std::unexpected("arcxel: window failed to initialise");
    }

    SetTargetFPS(info.target_fps); // 0 leaves the frame rate uncapped

    return std::expected<Window, std::string>(std::in_place, Token{});
}

static auto run() -> arcxel::Fallible {

    const auto winfo = arcxel::WindowInfo{.width = WIDTH, .height = HEIGHT, .target_fps = 0};

    if (auto r = create_window(winfo); !r) {
        return r;
    }


    // Before the window, so raylib's start-up messages reach the file too.
    const auto stem = arcxel::timing::begin_run();


    if (!window) {
        return std::unexpected(window.error());
    }

    log::Logger::instance().log(log::LogLevel::Info, "window opened {}x{}", info.width, info.height);

    DisableCursor();

    auto engine = arcxel::Engine();

    // ~4M samples, about ten minutes of uncapped frames 
    // Samples past this are dropped, not reallocated
    arcxel::timing::reserve(1U << 22U);

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

    EnableCursor();

    arcxel::timing::log_summary();

    if (arcxel::timing::write_run_csv().empty()) {
        return std::unexpected("timing: could not write this run's samples");
    }

    return {};
}

auto main() -> int {
    auto syncerr = std::osyncstream(std::cerr);

    if constexpr (arcxel::log::logging_enabled) {

        auto file = create_log_file(syncerr);

        auto&& os = create_logging_stream(file, syncerr);

        if 


        arcxel::log::capture_raylib_logs();
    } else {
        SetTraceLogLevel(LOG_NONE);
    }
    
    if constexpr (arcxel::timing::profiling_enabled) {
    
    }

    if constexpr (arcxel::log::logging_enabled || arcxel::timing::profiling_enabled) {
        create_output_dir() 
    }

    if (const auto result = run(); !result) {
        arcxel::log::log(syncerr, log::LogLevel::Fatal, "{}", result.error());
        std::exit(-1);
    }

    std::exit(0);
}
