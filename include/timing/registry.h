// <timing/registry.h> -*- C++ -*-

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

#ifndef ARCXEL_TIMING_REGISTRY_H
#define ARCXEL_TIMING_REGISTRY_H

#include <timing/clock.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace arcxel::timing {

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

// Sets sample capacity. Recording never allocates, so samples past this are
// discarded and counted instead.
auto reserve(std::size_t count) -> void;

// Creates <directory>/<yyyy-mm-dd>/ and returns this run's path stem, e.g.
// "results/2026-08-08/arcxel-timing-122241". Everything a run produces shares
// it, so the log and the samples always carry the same timestamp.
auto begin_run(std::string_view directory = "results") -> std::string;

// ---- Results. Not on the hot path. ----

auto samples() noexcept -> const std::vector<Sample>&;
auto dropped() noexcept -> std::size_t;
auto clear() noexcept -> void;

auto log_summary() -> void;

// Writes to an explicit path, replacing anything already there.
auto write_csv(std::string_view path) -> bool;

// Writes to <directory>/<yyyy-mm-dd>/arcxel-timing-<hhmmss>.csv in local time,
// creating the directories if needed, so runs never overwrite each other.
// Returns the path written, or an empty string on failure.
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

} // namespace arcxel::timing

#endif // ARCXEL_TIMING_REGISTRY_H
