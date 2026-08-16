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
        : speed(5.0f)
        , sprint_speed_scale(1.75f)
        , look_sensitivity(0.0015f) {
        camera.position = Vector3Zero();
        camera.target = Vector3{0.0f, 0.0f, -1.0f};
        camera.up = Vector3{0.0f, 1.0f, 0.0f};
        camera.fovy = 45.0;
        camera.projection = CameraProjection::CAMERA_PERSPECTIVE;
    }

    ~Player() noexcept = default;

    [[nodiscard]] auto get_camera() -> Camera3D { return camera; }

    auto handle_events() -> void {}

    auto update(f64 delta) -> void {
        _look_controls(delta);
        _movement_controls(delta);
    }

    auto render(f64 delta) -> void {}

protected:
    auto _movement_controls(f64 delta) -> void {
        auto speed_delta = speed * static_cast<f32>(delta);

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            speed_delta *= sprint_speed_scale;
        }

        if (IsKeyDown(KEY_W)) {
            CameraMoveForward(&camera, speed_delta, false);
        }

        if (IsKeyDown(KEY_A)) {
            CameraMoveRight(&camera, -speed_delta, false);
        }

        if (IsKeyDown(KEY_S)) {
            CameraMoveForward(&camera, -speed_delta, false);
        }

        if (IsKeyDown(KEY_D)) {
            CameraMoveRight(&camera, speed_delta, false);
        }

        if (IsKeyDown(KEY_Q)) {
            CameraMoveUp(&camera, -speed_delta);
        }

        if (IsKeyDown(KEY_E)) {
            CameraMoveUp(&camera, speed_delta);
        }
    }

    auto _look_controls(f64 delta) -> void {
        auto mouse = GetMouseDelta();
        CameraYaw(&camera, -mouse.x * look_sensitivity, false);
        CameraPitch(&camera, -mouse.y * look_sensitivity, true, false, false);
    }

private:
    Camera3D camera;

    f32 speed;
    f32 sprint_speed_scale;
    f32 look_sensitivity;
}; // class Player

} // namespace arcxel
