// <utils.h> -*- C++ -*-

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

#include <raylib.h>

#include <chrono>
#include <expected>
#include <filesystem>
#include <raymath.h>
#include <string>

namespace arcxel {

/*
 * @brief Unit Transform object, translation & rotation are zeroed and scale is one'd
 */
static constexpr Transform TransformIdentity = Transform{
    .translation = Vector3Zeros,
    .rotation = Quaternion{ .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f },
    .scale = Vector3Ones
};

/**
 * @brief Create a directory from the given string
 */
[[nodiscard]] auto create_dir(const std::string_view dirname)
    -> std::expected<std::filesystem::path, std::string>;


/**
 * @brief Obtain the current localised date and time as a time_point<>
 */
[[nodiscard]] auto current_datetime()
    -> std::chrono::local_time<std::chrono::system_clock::duration>;


[[nodiscard]] auto as(const rp3d::Vector3& vec) -> Vector3;


[[nodiscard]] auto as(const Vector3& vec) -> rp3d::Vector3;


[[nodiscard]] auto as(const rp3d::Quaternion& quat) -> Quaternion;


[[nodiscard]] auto as(const Quaternion& quat) -> rp3d::Quaternion;


[[nodiscard]] auto as(const rp3d::Transform& transform) -> Transform;


[[nodiscard]] auto as(const Transform& transform) -> rp3d::Transform;

} // namespace arcxel
