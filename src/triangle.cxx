// <triangle.cxx> -*- C++ -*-

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

#include <triangle.h>

#include <raylib.h>

namespace arcxel {

Triangle::Triangle(Vector2 v1, Vector2 v2, Vector2 v3, Color colour)
    : v1(v1)
    , v2(v2)
    , v3(v3)
    , colour(colour) {}

auto Triangle::draw() -> void { DrawTriangle(v1, v2, v3, colour); }

} // namespace arcxel
