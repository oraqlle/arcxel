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
#include <format>
#include <string_view>
#include <thread>
#include <vector>

namespace arcxel {

#ifdef ARCXEL_PROFILING
static constexpr bool profiling_enabled = true;
#else
static constexpr bool profiling_enabled = false;
#endif


struct Sample {

    // Monotonic clock only measures durations ie. it cannot tell you the time of day
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    using ThreadId = std::thread::id;

    enum class Label : u8 {
        Frame = 0,
        Events,
        Update,
        Render,
        Construct,
        Draw,
        Present
    }; // enum class Label


    TimePoint start;
    TimePoint end;
    ThreadId tid;
    Label label;
}; // struct Sample


constexpr u8 num_labels = static_cast<u8>(Sample::Label::Present) + 1;


static_assert(
    Sample::Clock::is_steady, "Profiling measurements require a monotonic clock"
);


[[nodiscard]] inline constexpr auto sample_label_to_depth(Sample::Label label) -> u32 {
    switch (label) {
        case Sample::Label::Construct:
        case Sample::Label::Draw:
        case Sample::Label::Present:
            return 2;

        case Sample::Label::Events:
        case Sample::Label::Update:
        case Sample::Label::Render:
            return 1;

        case Sample::Label::Frame:
        default:
            return 0;
    }
}


struct SampleRecord {
public:
    explicit SampleRecord(usize max_num_samples) noexcept;

    auto record(const Sample& sample) -> bool;

    [[nodiscard]] auto samples() -> const std::vector<Sample>&;

    [[nodiscard]] auto samples() const -> const std::vector<Sample>&;

    [[nodiscard]] constexpr auto dropped() -> usize;

    [[nodiscard]] constexpr auto dropped() const -> usize;

    [[nodiscard]] constexpr auto max_num_samples() -> usize;

    [[nodiscard]] constexpr auto max_num_samples() const -> usize;

    [[nodiscard]] auto write_timings_to_csv(std::string_view pathstr) -> Fallible;

private:
    std::vector<Sample> samples_store;
    usize max_samples;
    usize num_dropped_samples;
}; // class SampleRecord


// Times the enclosing scope and records one sample when it ends
class Timespan {
public:
    explicit Timespan(Sample::Label label, SampleRecord& store) noexcept
        : label(label)
        , start(Sample::Clock::now())
        , store(store) {}


    ~Timespan() noexcept {
        store.record(
            Sample{
                .start = start,
                .end = Sample::Clock::now(),
                .tid = std::this_thread::get_id(),
                .label = label
            }
        );
    }

    Timespan(const Timespan&) = delete;
    Timespan(Timespan&&) = delete;

    auto operator=(const Timespan&) -> Timespan& = delete;
    auto operator=(Timespan&&) -> Timespan& = delete;


private:
    Sample::Label label;
    Sample::TimePoint start;
    SampleRecord& store;
}; // class Timespan


/**
 * @brief Log summary of recorded samples
 */
auto log_trace_summary(const SampleRecord& store) -> void;


} // namespace arcxel


namespace std {

template <typename CharT>
struct formatter<arcxel::Sample::Label, CharT>
    : public std::formatter<basic_string_view<CharT>, CharT> {

    using fmttr_t = std::formatter<basic_string_view<CharT>, CharT>;

    static inline constexpr auto label_as_string(const arcxel::Sample::Label label)
        -> string_view {
        switch (label) {
            case arcxel::Sample::Label::Frame:
                return "Frame";
            case arcxel::Sample::Label::Events:
                return "Events";
            case arcxel::Sample::Label::Update:
                return "Update";
            case arcxel::Sample::Label::Render:
                return "Render";
            case arcxel::Sample::Label::Construct:
                return "Construct";
            case arcxel::Sample::Label::Draw:
                return "Draw";
            case arcxel::Sample::Label::Present:
                return "Present";
            default:
                return "UNKNOWN";
        }
    }


    template <typename FmtContext>
    auto format(arcxel::Sample::Label label, FmtContext& ctx) const
        -> FmtContext::iterator {
        return fmttr_t::format(label_as_string(label), ctx);
    }
}; // struct formatter<arcxel::Sample::Label, CharT>

} // namespace std
