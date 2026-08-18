// <transform.h> -*- C++ -*-

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

#include "object.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

[[nodiscard]] auto Vector3Sqrt(Vector3 v) -> Vector3;

[[nodiscard]] auto QuaternionConjugate(Quaternion q) -> Quaternion;

[[nodiscard]] auto QuaternionVector3Multiply(Quaternion q, Vector3 v) -> Vector3;

struct Transform3D : public Object {
    Transform3D() noexcept;

    ~Transform3D() noexcept = default;

    auto translate(Vector3 amount) -> void;

    auto rotate(Quaternion amount) -> void;

    auto rotate(Vector3 axis, f32 degrees) -> void;

    auto scale(Vector3 amount) -> void;

    auto look_at(Vector3 direction) -> void;

    [[nodiscard]] auto forward() -> Vector3;

    [[nodiscard]] auto up() -> Vector3;

    [[nodiscard]] auto right() -> Vector3;

    [[nodiscard]] auto transform_matrix() -> Matrix;

    Vector3 position;
    Quaternion rotation;
    Vector3 scaler;
}; // struct Transform3D

} // namespace arcxel
