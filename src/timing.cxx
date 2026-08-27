// <timing.cxx> -*- C++ -*-

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

#include "timing.h"
#include "log.h"
#include "types.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <print>

namespace arcxel {

static inline constexpr auto as_ms(Sample::Duration duration) -> f64 {
    return std::chrono::duration<double, std::milli>(duration).count();
}


static inline constexpr auto as_us(Sample::Duration duration) -> f64 {
    return std::chrono::duration<double, std::micro>(duration).count();
}


struct Totals {
    Sample::Label label;
    usize count = 0;
    Sample::Duration total = Sample::Duration::zero();
    Sample::Duration min = Sample::Duration::max();
    Sample::Duration max = Sample::Duration::zero();
};


static inline auto totals = std::array<Totals, num_labels>{
    Totals{.label = Sample::Label::Frame },
    Totals{.label = Sample::Label::Events },
    Totals{.label = Sample::Label::Update },
    Totals{.label = Sample::Label::Render },
    Totals{.label = Sample::Label::Construct },
    Totals{.label = Sample::Label::Draw  },
    Totals{.label = Sample::Label::Present }
};


auto log_trace_summary(const SampleRecord& store) -> void {
    if (store.samples().empty()) {
        log(LogLevel::Warning, "timing: no samples recorded");
        return;
    }

    for (const auto& sample : store.samples()) {
        const auto elapsed = sample.end - sample.start;
        const auto idx = static_cast<u8>(sample.label);
        auto& entry = totals[idx];

        entry.count += 1;
        entry.total += elapsed;
        entry.min = std::min(entry.min, elapsed);
        entry.max = std::max(entry.max, elapsed);
    }

    log(LogLevel::Info, "timing: {} samples, {} dropped, {} labels", store.samples().size(), store.dropped(), num_labels);
    log(LogLevel::Info, "timing: {:<16} {:>8} {:>12} {:>12} {:>12} {:>12}", "label", "count", "total/ms", "mean/us", "min/us", "max/us");

    for (const auto& entry : totals) {
        if (entry.count == 0) {
            continue;
        }

        log(LogLevel::Info,
            "timing: {:<16} {:>8} {:>12.3f} {:>12.3f} {:>12.3f} {:>12.3f}",
            static_cast<u8>(entry.label),
            entry.count,
            as_ms(entry.total),
            as_us(entry.total) / static_cast<f64>(entry.count),
            as_us(entry.min),
            as_us(entry.max));
    }
}


[[nodiscard]] auto
write_timings_to_csv(const SampleRecord& store, const std::filesystem::path& path)
    -> Fallible {
    if (store.samples().empty()) {
        return std::unexpected{make_log_string(
            LogLevel::Warning, "timing: nothing to write to '{}'", path.string()
        )};
    }

    const auto now = std::chrono::system_clock::now();
    const auto second = std::chrono::floor<std::chrono::seconds>(now);
    const auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - second).count();

    const auto fname = std::format("arcxel-{:%Y-%m-%d_%H:%M:%S}.{:03}", now, millis);
    const auto fpath = path / fname;

    // overwrite warning
    auto file = std::fstream(fpath, std::ios::trunc | std::ios::out);


    if (!file.is_open()) {
        return std::unexpected(make_log_string(
            LogLevel::Error, "timing: could not open {} for writing", fname
        ));
    }

    // CSV headings
    std::println(file, "label,depth,thread,start_ns,end_ns,duration_ns");

    // TODO: Sort samples first then save to CSV
    for (const auto& sample : store.samples()) {
        const auto start = sample.start.time_since_epoch();
        const auto end = sample.end.time_since_epoch();
        const auto diff = sample.end - sample.start;

        std::println(
            file,
            "{},{},{},{},{},{}",
            static_cast<u8>(sample.label),
            sample_label_to_depth(sample.label),
            sample.tid,
            as_us(start),
            as_us(end),
            as_us(diff)
        );
    }

    log(LogLevel::Info, "timing: wrote {} samples to {}", store.samples().size(), fname);

    return {};
}

} // namespace arcxel
