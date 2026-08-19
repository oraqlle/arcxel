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

#include "window.h"

#include "window_info.h"

#include <raylib.h>

#include <expected>
#include <string>
#include <utility>

namespace arcxel {

Window::Window(Token) noexcept {}

Window::~Window() { CloseWindow(); }

auto Window::create(const WindowInfo& info) -> std::expected<Window, std::string> {
    InitWindow(info.width, info.height, info.name.c_str());

    if (!IsWindowReady()) {
        // InitWindow may have got part way, so close before reporting.
        CloseWindow();
        return std::unexpected("arcxel: window failed to initialise");
    }

    SetTargetFPS(info.target_fps); // 0 leaves the frame rate uncapped

    return std::expected<Window, std::string>(std::in_place, Token{});
}

} // namespace arcxel
