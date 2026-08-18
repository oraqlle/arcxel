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
#include "log/logger.h"
#include "timing/registry.h"
#include "timing/span.h"
#include "types.h"
#include "window.h"
#include "window_info.h"

#include <raylib.h>

#include <exception>

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
// clang-format on

constexpr i32 WIDTH = 1920;
constexpr i32 HEIGHT = 1080;

namespace {

auto run() -> void {
    // Before the window, so raylib's start-up messages reach the file too.
    arcxel::log::set_file(arcxel::timing::begin_run() + ".log");

    // Uncapped: a frame rate cap would show up as vsync wait in every span.
    auto info = arcxel::WindowInfo{.width = WIDTH, .height = HEIGHT, .target_fps = 0};
    auto window = arcxel::Window(info);
    arcxel::log::info("window opened {}x{}", info.width, info.height);

    DisableCursor();

    auto engine = arcxel::Engine();

    // ~4M samples, about ten minutes of uncapped frames. Samples past this are
    // dropped, not reallocated.
    arcxel::timing::reserve(1U << 22U);
    const auto frame_label = arcxel::timing::register_label("frame");

    while (engine.is_running()) {
        const auto frame = arcxel::timing::Span(frame_label);

        engine.handle_events();

        const f64 delta = GetFrameTime();

        engine.update(delta);
        engine.render(delta);
    }

    EnableCursor();

    arcxel::timing::log_summary();
    arcxel::timing::write_run_csv();
}

} // namespace

auto main() -> int {
    arcxel::log::set_level_from_env();
    arcxel::log::adopt_raylib();

    try {
        run();
    } catch (const std::exception& error) {
        arcxel::log::fatal("{}", error.what());
        arcxel::log::close_file();
        return 1;
    } catch (...) {
        arcxel::log::fatal("terminated by an unknown exception");
        arcxel::log::close_file();
        return 1;
    }

    arcxel::log::close_file();
    return 0;
}
