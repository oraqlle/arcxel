// <triangle.h> -*- C++ -*-

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

#ifndef ARCXEL_TRIANGLE2D_H
#define ARCXEL_TRIANGLE2D_H

#include <object.h>

#include <raylib.h>

namespace arcxel {

class Triangle2D : Object {
private:
    Vector2 local_v1;
    Vector2 local_v2;
    Vector2 local_v3;

public:
    explicit Triangle2D(
        Vector2 local_v1, Vector2 local_v2, Vector2 local_v3, Color colour
    )
        : Object()
        , local_v1(local_v1)
        , local_v2(local_v2)
        , local_v3(local_v3) {
        this->position = Vector3(400.0f, 400.0f, 0.0f);
        this->colour = colour;
    }

    auto render() -> void {
        auto global_v1 = Vector2(position.x - local_v1.x, position.y - local_v1.y);
        auto global_v2 = Vector2(position.x + local_v2.x, position.y - local_v2.y);
        auto global_v3 = Vector2(position.x + local_v3.x, position.y - local_v3.y);
        DrawTriangle(global_v1, global_v2, global_v3, colour);
    }
}; // class Triangle2D

} // namespace arcxel

#endif // ARCXEL_TRIANGLE2D_H
