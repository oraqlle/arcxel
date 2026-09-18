// <workload.h> -*- C++ -*-

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

#include <cmath>
#include <ranges>

namespace arcxel {

    // per object update cost
    // shared by every architecture
    struct Workload {
        u32 magnitude = 0;   // mean iterations per object, 0 is off
        f32 variance = 0.0f; // 0 uniform, 1 maximally uneven
    }; // struct Workload


    // seeded by the caller so nothing can be precomputed
    [[nodiscard]] inline auto synthetic_work(
        [[maybe_unused]] u32 iterations, f32 seed
    ) noexcept -> f32 {
        // TODO
        // acc = seed
        // loop iterations times
        //     acc = fma(acc, 1.0000001f, 0.0000001f)
        // return acc
        //
        // each step needs the one before it
        // so the cpu cant run them at once
        // and cost scales with the count

        return seed; // stub, costs nothing yet
    }

} // namespace arcxel
