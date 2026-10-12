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
#include "types.h"
#include "workload.h"

#include <memory>
#include <raylib.h>
#include <vector>

namespace arcxel {

class Scene {
public:
    static constexpr u8 BASE_OBJ_COUNT = 20;

    static constexpr f32 MAZE_CENTRE_YPOS = 100.0f;
    static constexpr f32 MAZE_HEIGHT = 125.0f;
    static constexpr f32 MAZE_WIDTH = 50.0f;
    static constexpr f32 MAZE_DEPTH = 7.0f;

    static constexpr Color CONTAINER_COLOUR = GRAY;

    static constexpr Vector3 BOX_SIZE = Vector3{ 100.0f, 20.0f, 100.0f };
    static_assert(BOX_SIZE.x > 0.0f);
    static_assert(BOX_SIZE.y > 0.0f);
    static_assert(BOX_SIZE.z > 0.0f);

public:
    Scene() noexcept;

    explicit Scene(
        usize num_objects,
        Workload workload = Workload{}) noexcept;

    auto update(f64 delta) -> void;

    auto update_range(usize first, usize last, f64 delta) -> void;

    auto render(f64 delta) -> void;

    [[nodiscard]] auto primary_camera() -> Camera3D&;

    auto unload() -> void;

private:
    auto
    _M_add_plane(f32 length, f32 width, Vector3 position, Quaternion rotation, Color tint)
        -> void;

    auto _M_create_maze() -> void;

    auto _M_generate_transparent_face() -> void;

    auto _M_create_catchment_box() -> void;

    auto _M_generate_objects(usize num_objects, Workload workload) -> void;

public:
    std::vector<std::unique_ptr<GameObject>> objects;

}; // class Scene

} // namespace arcxel
