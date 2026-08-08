// <timing/clock.h> -*- C++ -*-

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

#ifndef ARCXEL_TIMING_CLOCK_H
#define ARCXEL_TIMING_CLOCK_H

#include <chrono>

namespace arcxel::timing {

// Monotonic and never adjusted, unlike system_clock which the logger uses.
// Measures durations only; it cannot tell you the time of day.
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;

static_assert(Clock::is_steady, "measurement requires a monotonic clock");

} // namespace arcxel::timing

#endif // ARCXEL_TIMING_CLOCK_H
