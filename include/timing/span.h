// <timing/span.h> -*- C++ -*-

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

#ifndef ARCXEL_TIMING_SPAN_H
#define ARCXEL_TIMING_SPAN_H

#include <timing/clock.h>
#include <timing/registry.h>

#include <cstdint>

// Set to 0 to compile every span away. Building both ways is how the
// profiler's own cost is measured.
#ifndef ARCXEL_PROFILING
#    define ARCXEL_PROFILING 1
#endif

namespace arcxel::timing {

// Times the enclosing scope and records one sample when it ends. Defined inline
// so the measurement does not pay for a call into the library.
class Span {
private:
    TimePoint start;
    LabelId label;
    std::uint16_t depth;

public:
    explicit Span(LabelId label) noexcept
        : start(Clock::now())
        , label(label)
        , depth(detail::depth++) {}

    Span(const Span&) = delete;
    Span(Span&&) = delete;
    auto operator=(const Span&) -> Span& = delete;
    auto operator=(Span&&) -> Span& = delete;

    ~Span() noexcept {
        const auto end = Clock::now();
        --detail::depth;
        record(Sample{start, end, label, depth, 0});
    }
};

} // namespace arcxel::timing

#define ARCXEL_TIMING_CAT_(a, b) a##b
#define ARCXEL_TIMING_CAT(a, b) ARCXEL_TIMING_CAT_(a, b)

#if ARCXEL_PROFILING
#    define ARCXEL_SPAN(label_id)                                                    \
        const ::arcxel::timing::Span ARCXEL_TIMING_CAT(arcxel_span_, __LINE__) {      \
            label_id                                                                 \
        }
#else
#    define ARCXEL_SPAN(label_id) ((void)0)
#endif

#endif // ARCXEL_TIMING_SPAN_H
