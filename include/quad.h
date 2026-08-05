// <quad.h> -*- C++ -*-

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

#ifndef ARCXEL_QUAD_H
#define ARCXEL_QUAD_H

#include <raylib.h>

namespace arcxel {

class Quad {
private:
    Rectangle rect;
    Color colour;

public:
    explicit Quad(Rectangle rect, Color colour)
        : rect(rect)
        , colour(colour) {}

    auto draw() -> void {
        //DrawRectangleRec(rect, colour);
        auto mix = Color{ .r = 255, .g = 255, .b = 0, .a = 255};
        DrawRectangleGradientEx(rect, RED, mix, BLUE, GREEN);
    }
}; // class Quad

} // namespace arcxel

#endif // ARCXEL_QUAD_H
