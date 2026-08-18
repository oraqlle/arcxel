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

#include "window_info.h"

#include <expected>
#include <string>

namespace arcxel {

class Window {
private:
    // Only Window can name this, so create() is the only way to obtain one. The
    // constructor has to be public for std::expected to build in place.
    struct Token {};

public:
    explicit Window(Token) noexcept;

    // raylib owns a single global window, so this guard is neither copyable
    // nor movable.
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    auto operator=(const Window&) -> Window& = delete;
    auto operator=(Window&&) -> Window& = delete;

    ~Window();

    // Opens the window, or says why it could not be opened. Nothing is left
    // open on failure.
    [[nodiscard]] static auto create(const WindowInfo& info)
        -> std::expected<Window, std::string>;

}; // class Window

} // namespace arcxel
