// <camera.h> -*- C++ -*-

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

#ifndef ARCXEL_CAMERA_H
#define ARCXEL_CAMERA_H

#include <component.h>
#include <transform.h>

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

class Camera {
public:
    Camera() noexcept
        : transform()
        , near{0.1f}
        , far{100.0f}
        , aspect{1.3333f}
        , primary(true)
        , cam{0} {
        cam.position = transform.position;
        cam.target = transform.forward();
        cam.up = transform.up();
        cam.fovy = 45.0 * DEG2RAD;
        cam.projection = CameraProjection::CAMERA_PERSPECTIVE;
    }

    ~Camera() noexcept = default;

    [[nodiscard]] auto view_matrix() noexcept -> Matrix { return GetCameraMatrix(cam); }

    [[nodiscard]] auto projection_matrix() noexcept -> Matrix {
        switch (cam.projection) {
            case CameraProjection::CAMERA_ORTHOGRAPHIC:
                // log unsupported, backup 'Perspective'
                [[fallthrough]];
            case CameraProjection::CAMERA_PERSPECTIVE:
            default:
                return MatrixPerspective(cam.fovy, aspect, near, far);
        }
    }

public:
    Transform3D transform;

    f32 near;
    f32 far;
    f32 aspect;

    bool primary;

private:
    Camera3D cam;
}; // class Camera

} // namespace arcxel

#endif // ARCXEL_CAMERA_H
