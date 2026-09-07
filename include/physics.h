// <physics.h> -*- C++ -*-

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

#include "rp3d.h"

#include <optional>

namespace arcxel {

class Physics {
public:
    [[nodiscard]] static auto
    singleton(std::optional<rp3d::PhysicsWorld::WorldSettings> init = std::nullopt)
        -> Physics&;

private:
    explicit Physics(
        std::optional<rp3d::PhysicsWorld::WorldSettings> opt_settings
    ) noexcept;

    ~Physics() noexcept;

public:
    rp3d::PhysicsCommon physics_common;    
    rp3d::PhysicsWorld* physics_world;    
};

} // namespace arcxel
