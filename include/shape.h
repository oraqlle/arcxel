// <shape.h> -*- C++ -*-

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
#include "physics.h"
#include "rp3d.h"
#include "utils.h"

#include <raylib.h>

namespace arcxel {

class Shape : public GameObject {
public:
    explicit Shape(rp3d::BodyType btype, Transform transform, Color colour) noexcept;

    virtual ~Shape() noexcept;

    virtual auto handle_events() -> void override;

    virtual auto update(f64 delta) -> void override;

    virtual auto render(f64 delta) -> void override;

protected:
    static auto _M_create_rigid_body(rp3d::BodyType btype, Transform transform)
        -> rp3d::RigidBody* {
        const auto physics_pos = as(transform.translation);
        auto physics_transform =
            rp3d::Transform{physics_pos, rp3d::Quaternion::identity()};
        auto body = Physics::singleton().world->createRigidBody(physics_transform);
        body->setType(btype);
        return body;
    }

    virtual auto _M_create_mesh() -> void = 0;

    virtual auto _M_create_collision_shape() -> void = 0;

protected:
    Color colour;
    Mesh mesh;
    Model model;

    rp3d::RigidBody* body;
    rp3d::CollisionShape* shape;
    rp3d::Collider* collider;
}; // class Cube

} // namespace arcxel
