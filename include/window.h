// <window.h> -*- C++ -*-

//  Arcxel Test Bench
//  Copyright (C) 2026  Tyler Swann, Georgia Kannelis
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

#ifndef ARCXEL_WINDOW_H
#define ARCXEL_WINDOW_H

#include <string>

namespace arcxel {

class Window {
private:
    int width;
    int height;
    int target_fps;
    std::string window_name;

public:
    explicit Window(
        int w, int h, std::string win_name = "raylib window", int target_fps = 0
    );

    // raylib owns a single global window, so this guard is neither copyable
    // nor movable.
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    auto operator=(const Window&) -> Window& = delete;
    auto operator=(Window&&) -> Window& = delete;

    ~Window() noexcept;
};

} // namespace arcxel

#endif // ARCXEL_WINDOW_H
