// <window.h> -*- C++ -*-

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

#include <types.h>

#include <raylib.h>

#include <string>

namespace arcxel {

struct Window {
public:
    Window(Window&& window) noexcept = default;

    Window(const Window& window) noexcept = delete;

    [[nodiscard]] auto operator=(Window&& window) noexcept -> Window& = default;

    auto operator=(const Window& window) noexcept -> Window& = delete;

    explicit Window(
        i32 w, i32 h, std::string win_name = "raylib window", i32 target_fps = 60
    ) noexcept
        : width(w)
        , height(h)
        , target_fps(target_fps)
        , window_name(win_name) {
        InitWindow(w, h, window_name.c_str());
        SetTargetFPS(target_fps);
    }

    ~Window() noexcept { CloseWindow(); }

private:
    i32 width;
    i32 height;
    i32 target_fps;
    std::string window_name;
}; // struct Window

} // namespace arcxel
