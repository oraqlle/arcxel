// <window.cxx> -*- C++ -*-

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

#include <window.h>

#include <raylib.h>

#include <stdexcept>
#include <string>

namespace arcxel {

Window::Window(int w, int h, std::string win_name, int target_fps)
    : width(w)
    , height(h)
    , target_fps(target_fps)
    , window_name(win_name) {
    InitWindow(w, h, window_name.c_str());

    if (!IsWindowReady()) {
        throw std::runtime_error("arcxel: window failed to initialise");
    }

    SetTargetFPS(target_fps); // 0 leaves the frame rate uncapped
}

Window::~Window() noexcept { CloseWindow(); }

} // namespace arcxel
