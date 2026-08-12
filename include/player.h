// <player.h> -*- C++ -*-

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

#include <types.h>

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

namespace arcxel {

class Player {
public:
    Player() noexcept
        : camera{0}
        , speed(5.0)
        , look_sensitivity(0.0015) {
        camera.position = Vector3{0.0};
        camera.target = Vector3{0.0, 0.0, -1.0};
        camera.up = Vector3{0.0, 1.0, 0.0};
        camera.fovy = 45.0 * DEG2RAD;
        camera.projection = CameraProjection::CAMERA_PERSPECTIVE;
    }

    ~Player() noexcept = default;

    [[nodiscard]] auto get_camera() -> Camera3D {
        return camera;
    }

    auto handle_events() -> void {}

    auto update(f64 delta) -> void {
        auto move = _movement_controls(delta);
        auto rotation = _look_controls(delta);
        UpdateCameraPro(camera, move, rotation, 1.0);
    }

    auto render(f64 delta) -> void {}

protected:
    [[nodiscard]] auto _movement_controls(f64 delta) -> Vector3 {
        auto key = GetKeyPressed();
        auto velocity = speed * delta;

        switch (key) {
            case KEY_W:
                return Vector3Scale(GetCameraForward(camera), velocity);
            case KEY_A:
                return Vector3Scale(GetCameraRight(camera), -velocity);
            case KEY_S:
                return Vector3Scale(GetCameraForward(camera), -velocity);
            case KEY_D:
                return Vector3Scale(GetCameraRight(camera), velocity);
            default:
                return Vector3{0.0};
        }
    }

    [[nodiscard]] auto _look_controls(f64 delta) -> Vector3 {
        auto mouse = GetMouseDelta();
        auto pitch_delta = -mouse.y * look_sensitivity;
        auto yaw_delta = mouse.x * look_sensitivity;
        return {yaw_delta, pitch_delta, 1.0};
    }

private:
    Camera3D camera;

    f32 speed;
    f32 look_sensitivity;
}; // class Player

} // namespace arcxel
