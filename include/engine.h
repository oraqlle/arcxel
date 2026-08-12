// <engine.h> -*- C++ -*-

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

#pragma once

#include <scene.h>
#include <types.h>
#include <window_info.h>

#include <raylib.h>

namespace arcxel {

class Engine {
public:
    Engine() = delete;

    explicit Engine(const WindowInfo& winfo) noexcept
        : window_info(winfo)
        , running(true) {
        InitWindow(winfo.width, winfo.height, winfo.name.c_str());
        SetTargetFPS(winfo.target_fps);
    }

    ~Engine() noexcept { CloseWindow(); };

    // make singleton

    [[nodiscard]] auto is_running() -> bool { return running && !WindowShouldClose(); }

    auto stop() -> void { running = false; }

    auto handle_events() -> void { scene.handle_events(); }

    auto update(f64 delta) -> void { scene.update(delta); }

    auto render(f64 delta) -> void {
        auto camera = scene.primary_camera();

        BeginDrawing();

        BeginMode3D(camera);

        scene.render(delta);

        EndMode3D();

        EndDrawing();
    }

public:
    WindowInfo window_info;

private:
    bool running;
    Scene scene;

}; // class Engine

} // namespace arcxel
