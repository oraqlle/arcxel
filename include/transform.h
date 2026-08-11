// <transform.h> -*- C++ -*-

//  Arcxel Test Bench
//  Copyright (C) 2026  Tyler Swann, Georgia Kannelis
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

#ifndef ARCXEL_TRANSFORM_H
#define ARCXEL_TRANSFORM_H

#include <object.h>

#include <raylib.h>
#include <raymath.h>

#include <cmath>

namespace arcxel {

[[nodiscard]] auto Vector3Sqrt(Vector3 v) -> Vector3 {
    return Vector3{.x = std::sqrt(v.x), .y = std::sqrt(v.y), .z = std::sqrt(v.z)};
}

[[nodiscard]] auto QuaternionConjugate(Quaternion q) -> Quaternion {
    return {.x = -q.x, .y = -q.y, .z = -q.z, .w = q.w};
}

[[nodiscard]] auto QuaternionVector3Multiply(Quaternion q, Vector3 v) -> Vector3 {
    auto pq = Quaternion{0.0f, q.x, q.y, q.z};
    auto nq = QuaternionNormalize(q);
    auto cq = QuaternionConjugate(q);
    auto nq_pq = QuaternionMultiply(nq, pq);
    auto result = QuaternionMultiply(nq_pq, cq);

    return {result.x, result.y, result.z};
}

struct Transform3D : public Object {
    Transform3D() noexcept
        : position{0.0f}
        , rotation{1.0f, 0.0f, 0.0f, 0.0f}
        , scaler{1.0f} {}

    ~Transform3D() noexcept = default;

    auto translate(Vector3 amount) -> void {
        position = Vector3Add(position, amount);
    }

    auto rotate(Quaternion amount) -> void {
        rotation = QuaternionMultiply(rotation, amount);
    }

    auto rotate(Vector3 axis, f32 degrees) -> void {}

    auto scale(Vector3 amount) -> void {
        scaler = Vector3Multiply(scaler, amount);
    }

    auto look_at(Vector3 direction) -> void {
        auto dot = Vector3DotProduct(Vector3{0.0f, 0.0f, -1.0f}, direction);
        if (dot > 0.999999 || dot < -0.999999) {
            rotation = Quaternion{0.0f, 0.0f, 0.0f, 1.0f};
            return;
        }

        auto a = Vector3CrossProduct(Vector3{0.0f, 0.0f, -1.0f}, direction);
        auto v1_len = Vector3Length(Vector3{0.0f, 0.0f, -1.0f});
        auto v2_len = Vector3Length(direction);
        auto w = std::sqrt((v1_len * v1_len) * (v2_len * v2_len)) + dot;

        rotation.x = a.x;
        rotation.y = a.y;
        rotation.z = a.z;
        rotation.w = w;
    }

    [[nodiscard]] auto forward() -> Vector3 {
        return QuaternionVector3Multiply(rotation, Vector3{0.0f, 0.0f, -1.0f});
    }

    [[nodiscard]] auto up() -> Vector3 {
        return QuaternionVector3Multiply(rotation, Vector3{0.0f, 1.0f, 0.0f});
    }

    [[nodiscard]] auto right() -> Vector3 {
        return QuaternionVector3Multiply(rotation, Vector3{1.0f, 0.0f, 0.0f});
    }

    [[nodiscard]] auto transform_matrix() -> Matrix {
        auto t = MatrixTranslate(position.x, position.y, position.z);
        auto s = MatrixScale(scaler.x, scaler.y, scaler.z);
        auto r = QuaternionToMatrix(rotation);

        auto model = MatrixMultiply(t, MatrixMultiply(r, s));
        return model;
    }

    Vector3 position;
    Quaternion rotation;
    Vector3 scaler;
}; // struct Transform3D

} // namespace arcxel

#endif // ARCXEL_TRANSFORM_H
