// <sphere.h> -*- C++ -*-

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

#include "physics_object.h"
#include "rp3d.h"
#include "utils.h"

#include <raylib.h>

namespace arcxel {

class Sphere : public PhysicsObject {
public:
    explicit Sphere(Transform transform = TransformIdentity, Color colour = GRAY) noexcept;

    ~Sphere() noexcept override;

protected:
    virtual auto _M_create_mesh() -> void override;

    virtual auto _M_create_collision_shape() -> void override;

protected:
    rp3d::SphereShape* shape;
    rp3d::Collider* collider;
    f32 radius;
}; // class Sphere

} // namespace arcxel
