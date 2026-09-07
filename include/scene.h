// <scene.h> -*- C++ -*-

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

#include "game_object.h"
#include "player.h"
#include "types.h"

#include <memory>
#include <raylib.h>
#include <vector>

namespace arcxel {

class Scene {
public:
    Scene() noexcept;

    explicit Scene(usize num_objects) noexcept;

    auto handle_events() -> void;

    auto update(f64 delta) -> void;

    auto render(f64 delta) -> void;

    [[nodiscard]] auto primary_camera() -> Camera3D;

private:
    auto _M_create_player() -> void;

    auto _M_generate_objects(usize num_objects) -> void;

public: // Scene objects
    std::vector<std::unique_ptr<GameObject>> objects;

private:
    Player player;

}; // class Scene

} // namespace arcxel
