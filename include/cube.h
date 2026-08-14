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
    Cube() noexcept
        : GameObject()
        , width(1.0f)
        , height(1.0f)
        , length(1.0f)
        , colour(RED) {
        mesh = GenMeshCube(width, height, length);
        model = LoadModelFromMesh(mesh);
    };

    explicit Cube(Transform3D transform) noexcept
        : GameObject(transform)
        , width(1.0f)
        , height(1.0f)
        , length(1.0f)
        , colour(RED) {
        this->transform = transform;
        mesh = GenMeshCube(width, height, length);
        model = LoadModelFromMesh(mesh);
    }

    ~Cube() noexcept = default;

    auto update(f64 delta) -> void {}

    auto render(f64 delta) -> void { DrawModel(model, transform.position, 1.0f, colour); }

private:
    f32 width;
    f32 height;
    f32 length;
    Color colour;
    Mesh mesh;
    Model model;

}; // class Cube

} // namespace arcxel
