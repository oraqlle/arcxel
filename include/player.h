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
        , look_sensitivity(0.03f) {
        camera.position = Vector3{0.0f, 10.0f, 10.0f};
        camera.target = Vector3{0.0f, 0.0f, 0.0f};
        camera.up = Vector3{0.0f, 1.0f, 0.0f};
        camera.fovy = 45.0;
        camera.projection = CameraProjection::CAMERA_PERSPECTIVE;
    }

    ~Player() noexcept = default;

    [[nodiscard]] auto get_camera() -> Camera3D { return camera; }

    auto handle_events() -> void {}

    auto update(f64 delta) -> void {
        // auto move = Vector3Zero();
        //auto rotation = Vector3Zero();

        auto move = _movement_controls(delta);
        auto rotation = _look_controls(delta);
        UpdateCameraPro(&camera, move, rotation, 0.0);
    }

    auto render(f64 delta) -> void {}

protected:
    [[nodiscard]] auto _movement_controls(f64 delta) -> Vector3 {
        auto velocity = speed * static_cast<f32>(delta);

        switch (GetKeyPressed()) {
            case KEY_W:
                return Vector3{velocity, 0.0f, 0.0f};
            case KEY_A:
                return Vector3{0.0f, -velocity, 0.0f};
            case KEY_S:
                return Vector3{-velocity, 0.0f, 0.0f};
            case KEY_D:
                return Vector3{0.0f, velocity, 0.0f};
            default:
                return Vector3Zero();
        }
    }

    [[nodiscard]] auto _look_controls(f64 delta) -> Vector3 {
        auto mouse = GetMouseDelta();
        auto pitch_delta = mouse.y * look_sensitivity;
        auto yaw_delta = mouse.x * look_sensitivity;
        return {yaw_delta, pitch_delta, 0.0f};
    }

private:
    Camera3D camera;

    f32 speed;
    f32 look_sensitivity;
}; // class Player

} // namespace arcxel
