// <entity.h> -*- C++ -*-

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

#include "component.h"
#include "object.h"
#include "transform.h"
#include "types.h"

#include <raylib.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace arcxel {

class Entity : public Object {
public:
    Entity() noexcept
        : id(0)
        , name("")
        , transform() {}

    ~Entity() noexcept = default;

    template <typename T>
    [[nodiscard]] auto create_component() -> std::weak_ptr<T> {
        auto comp = std::make_shared<T>();
        components.push_back(comp);
        return comp;
    }

    template <typename T>
    [[nodiscard]] auto find_component() -> std::optional<std::weak_ptr<T>> {
        for (auto comp : components) {
            auto ptr = std::dynamic_pointer_cast<T>(comp);

            if (ptr) {
                return std::make_optional<std::weak_ptr<T>>(ptr);
            }
        }

        return std::nullopt;
    }

    template <typename T>
    auto remove_component(std::weak_ptr<T> comp) -> void {
        auto it = std::ranges::find(components, comp);

        if (it != components.end()) {
            components.erase(it);
        }
    }

public:
    u32 id;
    std::string name;
    Transform3D transform;

protected:
    std::vector<std::shared_ptr<Component>> components;
}; // class Entity

} // namespace arcxel
