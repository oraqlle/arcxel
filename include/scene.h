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

#include <reactphysics3d/reactphysics3d.h>

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
    static constexpr f64 fixed_dt = 1.0 / 60.0;

    Scene() noexcept;

    ~Scene() noexcept;

    // Owns GPU resources, so copying it would free them twice.
    Scene(const Scene&) = delete;
    Scene(Scene&&) = delete;
    auto operator=(const Scene&) -> Scene& = delete;
    auto operator=(Scene&&) -> Scene& = delete;

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
    auto _build_container() -> void;

    auto _reset_sphere() -> void;

    auto _render_container() -> void;

    auto _render_sphere() -> void;

    // Declared before anything it allocates. Members destruct in reverse order,
    // so the factory has to outlive every world, shape and body it hands out.
    rp3d::PhysicsCommon physics;

    rp3d::PhysicsWorld* world;
    rp3d::RigidBody* container;    //< one static body, a collider per face
    rp3d::RigidBody* sphere;
    rp3d::SphereShape* sphere_shape;

    Vector3 interior;    //< half-extents of the space objects live in
    f32 wall_thickness;

    Mesh sphere_mesh;         //< one mesh, drawn once per sphere
    Material sphere_material;
    f32 sphere_radius;

}; // class Scene

} // namespace arcxel
