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
#include "window.h"
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

namespace log = arxcel::log;
// clang-format on

constexpr i32 WIDTH = 1920;
constexpr i32 HEIGHT = 1080;

auto run() -> std::expected<void, std::string> {
    // Before the window, so raylib's start-up messages reach the file too.
    const auto stem = arcxel::timing::begin_run();

    if (stem.empty()) {
        return std::unexpected("timing: could not create this run's directory");
    }

    // Uncapped: a frame rate cap would show up as vsync wait in every span.
    auto info = arcxel::WindowInfo{.width = WIDTH, .height = HEIGHT, .target_fps = 0};
    const auto window = arcxel::Window::create(info);

    if (!window) {
        return std::unexpected(window.error());
    }

    log::Logger::instance().log(log::LogLevel::Info, "window opened {}x{}", info.width, info.height);

    DisableCursor();

    auto engine = arcxel::Engine();

    // ~4M samples, about ten minutes of uncapped frames 
    // Samples past this are dropped, not reallocated
    arcxel::timing::reserve(1U << 22U);
    const auto frame_label = arcxel::timing::register_label("frame");
    const auto events_label = arcxel::timing::register_label("events");
    const auto update_label = arcxel::timing::register_label("update");
    const auto render_label = arcxel::timing::register_label("render");

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
    arcxel::log::capture_raylib_logs();

    if (const auto result = run(); !result) {
        log::Logger::instance().log(log::LogLevel::Fatal, "{}", result.error());
        std::exit(-1);
    }

    std::exit(0);
}
