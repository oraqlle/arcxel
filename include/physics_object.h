// <physics_object.h> -*- C++ -*-

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
#include "rp3d.h"

#include <raylib.h>

namespace arcxel {

class PhysicsObject : public GameObject {
public:
    explicit PhysicsObject(
        rp3d::BodyType btype,
        Transform transform,
        Color colour) noexcept;

    virtual ~PhysicsObject() noexcept;

    virtual auto handle_events() -> void override;

    virtual auto update(f64 delta) -> void override;

    virtual auto render(f64 delta) -> void override;

protected:
    virtual auto _M_create_mesh() -> void = 0;

    virtual auto _M_create_collision_shape() -> void = 0;

protected:
    Color colour;
    Mesh mesh;
    Model model;

    rp3d::RigidBody* body;
}; // class Cube

} // namespace arcxel
