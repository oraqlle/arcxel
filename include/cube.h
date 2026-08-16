// <cube.h> -*- C++ -*-

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

#include <game_object.h>

#include <raylib.h>

namespace arcxel {

class Cube : public GameObject {
public:
    Cube() noexcept;

    explicit Cube(Transform3D transform) noexcept;

    ~Cube() noexcept = default;

    auto update(f64 delta) -> void;

    auto render(f64 delta) -> void;

private:
    f32 width;
    f32 height;
    f32 length;
    Color colour;
    Mesh mesh;
    Model model;

}; // class Cube

} // namespace arcxel
