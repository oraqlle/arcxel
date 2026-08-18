// <timing/span.h> -*- C++ -*-

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

#ifndef ARCXEL_TIMING_SPAN_H
#define ARCXEL_TIMING_SPAN_H

#include <timing/clock.h>
#include <timing/registry.h>

#include <cstdint>
#include <type_traits>

#ifndef ARCXEL_PROFILING
#    define ARCXEL_PROFILING 1
#endif

namespace arcxel::timing {

inline constexpr bool profiling_enabled = ARCXEL_PROFILING != 0;

namespace detail {

// times the enclosing scope and records one sample
// defined inline so the measurement does call library
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

// Same interface, no storage and no clock reads 
// neither can rot while the other is selected
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

#endif // ARCXEL_TIMING_SPAN_H
