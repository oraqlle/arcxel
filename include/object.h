// <object.h> -*- C++ -*-

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

#ifndef ARCXEL_OBJECT_H
#define ARCXEL_OBJECT_H

#include <types.h>

#include <raylib.h>

#include <string>

namespace arcxel {

struct Object {
    struct Type {
        std::string classname;
        u32 classid;
    }; // struct Type

    Object() noexcept
        : classtype("", 0) {}

    virtual ~Object() = default;

    Object::Type type();

    Type classtype;
}; // struct Object

} // namespace arcxel

#endif // ARCXEL_OBJECT_H
