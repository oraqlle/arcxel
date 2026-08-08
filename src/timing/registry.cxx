// <timing/registry.cxx> -*- C++ -*-

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

#include <timing/registry.h>

#include <log/logger.h>
#include <timing/clock.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <format>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace arcxel::timing {

namespace {

std::vector<std::string> label_names;

auto to_ms(Duration duration) noexcept -> double {
    return std::chrono::duration<double, std::milli>(duration).count();
}

// Spans are routinely sub-microsecond, so milliseconds would print as 0.000.
auto to_us(Duration duration) noexcept -> double {
    return std::chrono::duration<double, std::micro>(duration).count();
}

struct Totals {
    std::size_t count = 0;
    Duration total = Duration::zero();
    Duration min = Duration::max();
    Duration max = Duration::zero();
};

} // namespace

auto register_label(std::string_view name) -> LabelId {
    const auto existing = std::ranges::find(label_names, name);

    if (existing != label_names.end()) {
        return static_cast<LabelId>(existing - label_names.begin());
    }

    label_names.emplace_back(name);
    return static_cast<LabelId>(label_names.size() - 1);
}

auto label_name(LabelId id) noexcept -> std::string_view {
    if (id < label_names.size()) {
        return label_names[id];
    }

    return "<unknown>";
}

auto label_count() noexcept -> std::size_t { return label_names.size(); }

auto reserve(std::size_t count) -> void { detail::store.reserve(count); }

auto samples() noexcept -> const std::vector<Sample>& { return detail::store; }

auto dropped() noexcept -> std::size_t { return detail::dropped_samples; }

auto clear() noexcept -> void {
    detail::store.clear();
    detail::dropped_samples = 0;
}

auto log_summary() -> void {
    if (detail::store.empty()) {
        log::warn("timing: no samples recorded");
        return;
    }

    auto totals = std::vector<Totals>(label_names.size());

    for (const auto& sample : detail::store) {
        if (sample.label >= totals.size()) {
            continue;
        }

        const auto elapsed = sample.end - sample.start;
        auto& entry = totals[sample.label];

        entry.count += 1;
        entry.total += elapsed;
        entry.min = std::min(entry.min, elapsed);
        entry.max = std::max(entry.max, elapsed);
    }

    log::info(
        "timing: {} samples, {} dropped, {} labels", detail::store.size(), dropped(),
        label_names.size()
    );
    log::info(
        "timing: {:<16} {:>8} {:>12} {:>12} {:>12} {:>12}", "label", "count",
        "total/ms", "mean/us", "min/us", "max/us"
    );

    for (std::size_t id = 0; id < totals.size(); ++id) {
        const auto& entry = totals[id];

        if (entry.count == 0) {
            continue;
        }

        log::info(
            "timing: {:<16} {:>8} {:>12.3f} {:>12.3f} {:>12.3f} {:>12.3f}",
            label_names[id], entry.count, to_ms(entry.total),
            to_us(entry.total) / static_cast<double>(entry.count), to_us(entry.min),
            to_us(entry.max)
        );
    }
}

auto write_csv(std::string_view path) -> bool {
    if (detail::store.empty()) {
        log::warn("timing: nothing to write to {}", path);
        return false;
    }

    const auto name = std::string(path);
    auto* file = std::fopen(name.c_str(), "w");

    if (file == nullptr) {
        log::error("timing: could not open {} for writing", name);
        return false;
    }

    // Times are nanoseconds relative to the earliest start, so the file carries
    // no dependence on the clock's arbitrary epoch. Samples are recorded when a
    // span ends, so the first row is not the earliest one.
    const auto origin = std::ranges::min(
        detail::store, {}, [](const Sample& sample) { return sample.start; }
    ).start;

    std::fputs("label,depth,thread,start_ns,end_ns,duration_ns\n", file);

    for (const auto& sample : detail::store) {
        const auto start_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(sample.start - origin)
                .count();
        const auto end_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(sample.end - origin)
                .count();

        std::fprintf(
            file, "%s,%u,%u,%lld,%lld,%lld\n",
            std::string(label_name(sample.label)).c_str(), sample.depth, sample.thread,
            static_cast<long long>(start_ns), static_cast<long long>(end_ns),
            static_cast<long long>(end_ns - start_ns)
        );
    }

    std::fclose(file);

    log::info("timing: wrote {} samples to {}", detail::store.size(), name);
    return true;
}

auto write_run_csv(std::string_view directory) -> std::string {
    // Local time, so the date folder matches the day the run happened. libc++
    // has no chrono time zone support yet, hence the C API.
    const auto stamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const auto* local = std::localtime(&stamp);

    if (local == nullptr) {
        log::error("timing: could not resolve local time");
        return {};
    }

    const auto dir =
        std::filesystem::path(directory)
        / std::format(
            "{:04}-{:02}-{:02}", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday
        );

    auto ec = std::error_code{};

    std::filesystem::create_directories(dir, ec);

    if (ec) {
        log::error("timing: could not create {}: {}", dir.string(), ec.message());
        return {};
    }

    const auto name = std::format(
        "arcxel-timing-{:02}{:02}{:02}.csv", local->tm_hour, local->tm_min, local->tm_sec
    );
    const auto path = (dir / name).string();

    if (!write_csv(path)) {
        return {};
    }

    return path;
}

} // namespace arcxel::timing
