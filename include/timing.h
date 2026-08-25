// <timing.h> -*- C++ -*-

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

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

// The build system supplies this; the fallback is for compiling without CMake.
#ifndef ARCXEL_PROFILING
#    define ARCXEL_PROFILING 1
#endif

namespace arcxel::timing {

// Monotonic and never adjusted, unlike system_clock which the logger uses
// Measures durations only -> it cannot tell you the time of day
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;

static_assert(Clock::is_steady, "measurement requires a monotonic clock");

inline constexpr bool profiling_enabled = ARCXEL_PROFILING != 0;

using LabelId = u16;

struct Sample {
    TimePoint start;
    TimePoint end;
    LabelId label;
    u16 depth;
    u32 thread; // always 0 until the game loop is parallelised
};

// ---- Set-up. Call during start-up, never inside a measured region. ----

// Registers a label, or returns the existing id if the name is already known.
[[nodiscard]] auto register_label(std::string_view name) -> LabelId;

// Sets sample capacity
// recording never allocates -> samples past this are discarded and counted
auto reserve(usize count) -> void;

// Creates <directory>/<yyyy-mm-dd>/ and returns the stem the log and CSV share.
[[nodiscard]] auto begin_run(std::string_view directory = "results") -> std::string;

// ---- Results. Not on the hot path. ----

auto log_summary() -> void;

// Writes this run's samples to <stem>.csv. Requires begin_run to have succeeded.
[[nodiscard]] auto write_run_csv() -> std::string;

namespace detail {

// Storage sits in the header so that record() can inline into the call site.
// Not thread safe: sharding by thread comes with the parallel game loop.
inline std::vector<Sample> store;
inline usize dropped_samples = 0;
inline thread_local u16 depth = 0;

} // namespace detail

// Hot path: no allocation, no locking, no I/O.
inline auto record(const Sample& sample) noexcept -> void {
    if (detail::store.size() < detail::store.capacity()) {
        detail::store.push_back(sample);
        return;
    }

    ++detail::dropped_samples;
}

// Times the enclosing scope and records one sample when it ends
class Span {
public:
    explicit Span(LabelId id) noexcept {
        if constexpr (profiling_enabled) {
            start = Clock::now();
            label = id;
            depth = detail::depth++;
        }
    }

    Span(const Span&) = delete;
    Span(Span&&) = delete;
    auto operator=(const Span&) -> Span& = delete;
    auto operator=(Span&&) -> Span& = delete;

    ~Span() noexcept {
        if constexpr (profiling_enabled) {
            const auto end = Clock::now();
            --detail::depth;
            record(Sample{start, end, label, depth, 0});
        }
    }

private:
    TimePoint start{};
    LabelId label{};
    u16 depth{};

}; // class Span

} // namespace arcxel::timing
