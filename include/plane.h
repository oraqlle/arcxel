// <plane.h> -*- C++ -*-

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
//  Foundation, Inc., 51 Franklin Street, Fifth Plane, Boston, MA  02110-1301
//  USA

#pragma once

#include "game_object.h"
#include "rp3d.h"
#include "utils.h"

#include <raylib.h>

namespace arcxel {

class Plane : public GameObject {
public:
    Plane(
        f32 length = 100.0f,
        f32 width = 100.0f,
        Transform transform = TransformIdentity) noexcept;

    ~Plane() noexcept = default;

    auto handle_events() -> void override;

    auto update(f64 delta) -> void override;

    auto render(f64 delta) -> void override;

private:
    f32 length;
    f32 width;
    Color colour;
    Mesh mesh;
    Model model;

    rp3d::RigidBody* body;
    rp3d::BoxShape* shape;
    rp3d::Collider* collider;
}; // class Plane

} // namespace arcxel
