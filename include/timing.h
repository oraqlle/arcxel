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

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
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

using LabelId = std::uint16_t;

struct Sample {
    TimePoint start;
    TimePoint end;
    LabelId label;
    std::uint16_t depth;
    std::uint32_t thread; // always 0 until the game loop is parallelised
};

// ---- Set-up. Call during start-up, never inside a measured region. ----

// Registers a label, or returns the existing id if the name is already known.
auto register_label(std::string_view name) -> LabelId;
auto label_name(LabelId id) noexcept -> std::string_view;
auto label_count() noexcept -> std::size_t;

// Sets sample capacity
// recording never allocates -> samples past this are discarded and counted
auto reserve(std::size_t count) -> void;

// Creates <directory>/<yyyy-mm-dd>/
auto begin_run(std::string_view directory = "results") -> std::string;

// ---- Results. Not on the hot path. ----

auto samples() noexcept -> const std::vector<Sample>&;
auto dropped() noexcept -> std::size_t;
auto clear() noexcept -> void;

auto log_summary() -> void;

// Writes to an explicit path, replacing anything already there.
auto write_csv(std::string_view path) -> bool;

// Writes to <directory>/<yyyy-mm-dd>/arcxel-timing-<hhmmss>.csv in local time
auto write_run_csv(std::string_view directory = "results") -> std::string;

namespace detail {

// Storage sits in the header so that record() can inline into the call site.
// Not thread safe: sharding by thread comes with the parallel game loop.
inline std::vector<Sample> store;
inline std::size_t dropped_samples = 0;
inline thread_local std::uint16_t depth = 0;

} // namespace detail

// Hot path: no allocation, no locking, no I/O.
inline auto record(const Sample& sample) noexcept -> void {
    if (detail::store.size() < detail::store.capacity()) {
        detail::store.push_back(sample);
        return;
    }

    ++detail::dropped_samples;
}

namespace detail {

// Times the enclosing scope and records one sample when it ends. Defined inline
// so the measurement does not pay for a call into the library.
class MeasuredSpan {
public:
    explicit MeasuredSpan(LabelId id) noexcept
        : start(Clock::now())
        , label(id)
        , depth(detail::depth++) {}

    MeasuredSpan(const MeasuredSpan&) = delete;
    MeasuredSpan(MeasuredSpan&&) = delete;
    auto operator=(const MeasuredSpan&) -> MeasuredSpan& = delete;
    auto operator=(MeasuredSpan&&) -> MeasuredSpan& = delete;

    ~MeasuredSpan() noexcept {
        const auto end = Clock::now();
        --detail::depth;
        record(Sample{start, end, label, depth, 0});
    }

private:
    TimePoint start;
    LabelId label;
    std::uint16_t depth;

}; // class MeasuredSpan

// Same interface, no storage and no clock reads. Both classes are always
// compiled, so neither can rot while the other is selected.
class DisabledSpan {
public:
    explicit DisabledSpan(LabelId) noexcept {}

    DisabledSpan(const DisabledSpan&) = delete;
    DisabledSpan(DisabledSpan&&) = delete;
    auto operator=(const DisabledSpan&) -> DisabledSpan& = delete;
    auto operator=(DisabledSpan&&) -> DisabledSpan& = delete;

    ~DisabledSpan() noexcept {}

}; // class DisabledSpan

} // namespace detail

// Construct one to time the enclosing scope:
//     const auto frame = timing::Span(frame_label);
using Span =
    std::conditional_t<profiling_enabled, detail::MeasuredSpan, detail::DisabledSpan>;

} // namespace arcxel::timing
