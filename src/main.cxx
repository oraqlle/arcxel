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

#include <log/logger.h>
#include <timing/registry.h>
#include <timing/span.h>
#include <triangle.h>
#include <window.h>

#include <raylib.h>

#include <exception>

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

namespace {

auto run() -> void {
    // Before the window, so raylib's start-up messages reach the file too.
    arcxel::log::set_file(arcxel::timing::begin_run() + ".log");

    auto window = arcxel::Window(WIDTH, HEIGHT, "Arcxel Window");
    arcxel::log::info("window opened {}x{}", WIDTH, HEIGHT);
    auto centre = Vector2{WIDTH / 2.0f, HEIGHT / 2.0f};

    auto v1 = Vector2{centre.x, centre.y - 175};
    auto v2 = Vector2{centre.x - 200, centre.y + 175};
    auto v3 = Vector2{centre.x + 200, centre.y + 175};
    auto tri = arcxel::Triangle(v1, v2, v3, ORANGE);

    // ~4M samples, about ten minutes of uncapped frames. Samples past this are
    // dropped, not reallocated.
    arcxel::timing::reserve(1U << 22U);
    const auto frame_span = arcxel::timing::register_label("frame");
    const auto begin_span = arcxel::timing::register_label("begin");
    const auto clear_span = arcxel::timing::register_label("clear");
    const auto draw_span = arcxel::timing::register_label("draw");
    const auto present_span = arcxel::timing::register_label("present");

    while (!WindowShouldClose()) {
        ARCXEL_SPAN(frame_span);

        {
            ARCXEL_SPAN(begin_span);
            BeginDrawing();
        }

        {
            ARCXEL_SPAN(clear_span);
            ClearBackground(RAYWHITE);
        }

        {
            ARCXEL_SPAN(draw_span);
            tri.draw();
        }

        {
            ARCXEL_SPAN(present_span);
            EndDrawing();
        }
    }

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
