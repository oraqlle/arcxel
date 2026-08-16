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
#include <player.h>

#include <raylib.h>

namespace arcxel {

class Scene {
public:
    Scene() noexcept {
        cube.transform.position.z = -15.0f;
}

    auto handle_events() -> void {
        cube.handle_events();
        player.handle_events();
    }

    auto update(f64 delta) -> void {
        cube.update(delta);
        player.update(delta);
    }

    auto render(f64 delta) -> void {
        DrawGrid(10000, 1.0f);
        cube.render(delta);
        player.render(delta);
    }

    [[nodiscard]] auto primary_camera() -> Camera3D { return player.get_camera(); }

public: // Scene objects
    Cube cube;
    Player player;

private:
    Camera3D primary_cam;

}; // class Scene

} // namespace arcxel
