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

#include "types.h"
#include "game_object.h"

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

namespace arcxel {

class Player : public GameObject {
public:
    explicit Player() noexcept;

    virtual ~Player() noexcept = default;

    [[nodiscard]] auto get_camera() -> Camera3D&;

    virtual auto update(f64 delta) -> void override;

    virtual auto render(f64 delta) -> void override;

protected:
    auto _movement_controls(f64 delta) -> void;

    auto _look_controls(f64 delta) -> void;

private:
    Camera3D camera;

    f32 speed;
    f32 sprint_speed_scale;
    f32 look_sensitivity;
}; // class Player

} // namespace arcxel
