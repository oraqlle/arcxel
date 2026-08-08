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

#include <raylib.h>

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
    )
        : width(w)
        , height(h)
        , target_fps(target_fps)
        , window_name(win_name) {
        InitWindow(w, h, window_name.c_str());
        SetTargetFPS(target_fps); // 0 leaves the frame rate uncapped
    }

    ~Window() noexcept { CloseWindow(); }
};

} // namespace arcxel

#endif // ARCXEL_WINDOW_H
