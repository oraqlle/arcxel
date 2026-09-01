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

#include "cube.h"
#include "player.h"
#include "types.h"

#include <raylib.h>

#include <array>

namespace arcxel {

class Scene {
public:
    // One face of the container, in half-extents because that is what rp3d takes. 
    struct Box {
        Vector3 centre;
        Vector3 half_extents;
    }; // struct Box

    static constexpr usize num_faces = 5; //< floor and four walls, no ceiling

    Scene() noexcept;

    auto handle_events() -> void;

    auto update(f64 delta) -> void;

    auto render(f64 delta) -> void;

    [[nodiscard]] auto primary_camera() -> Camera3D;

    // Floor first, then the four walls.
    [[nodiscard]] auto container_faces() const -> std::array<Box, num_faces>;

public: // Scene objects
    Cube cube;
    Player player;

private:
    auto _render_container() -> void;

    Vector3 interior;    //< half-extents of the space objects live in
    f32 wall_thickness;

}; // class Scene

} // namespace arcxel
