#include "timing.h"
#include "log.h"
#include "types.h"
#include "utils.h"

#include <algorithm>
#include <array>
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
    Totals{ .label = Sample::Label::Frame },
    Totals{ .label = Sample::Label::Events },
    Totals{ .label = Sample::Label::Update },
    Totals{ .label = Sample::Label::PhysicsUpdate },
    Totals{ .label = Sample::Label::Render },
    Totals{ .label = Sample::Label::Construct },
    Totals{ .label = Sample::Label::Draw },
    Totals{ .label = Sample::Label::Present }
};


auto log_trace_summary(const SampleRecord& store) -> void {
    if (store.samples().empty()) {
        log(LogLevel::Warning, "profiler: no samples recorded");
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

    log(LogLevel::Info,
        "profiler: {} samples, {} dropped, {} labels",
        store.samples().size(),
        store.dropped(),
        num_labels);

    log(LogLevel::Info,
        "profiler: {:<16} {:>8} {:>12} {:>12} {:>12} {:>12}",
        "label",
        "count",
        "total (ms)",
        "mean (us)",
        "min (us)",
        "max (us)");

    for (const auto& entry : totals) {
        if (entry.count == 0) {
            continue;
        }

        log(LogLevel::Info,
            "profiler: {:<16} {:>8} {:>12.3f} {:>12.3f} {:>12.3f} {:>12.3f}",
            entry.label,
            entry.count,
            as_ms(entry.total),
            as_us(entry.total) / static_cast<f64>(entry.count),
            as_us(entry.min),
            as_us(entry.max));
    }
}


SampleRecord::SampleRecord(usize max_num_samples) noexcept
    : max_samples(max_num_samples)
    , num_dropped_samples(0) {
    samples_store.reserve(max_num_samples);
}


auto SampleRecord::record(const Sample& sample) -> bool {
    if (samples_store.size() <= max_samples) {
        samples_store.emplace_back(sample);
        return true;
    }

    num_dropped_samples += 1;
    return false;
}


[[nodiscard]] auto SampleRecord::samples() -> const std::vector<Sample>& {
    return samples_store;
}


[[nodiscard]] auto SampleRecord::samples() const -> const std::vector<Sample>& {
    return samples_store;
}


[[nodiscard]] constexpr auto SampleRecord::dropped() -> usize {
    return num_dropped_samples;
}


[[nodiscard]] constexpr auto SampleRecord::dropped() const -> usize {
    return num_dropped_samples;
}


[[nodiscard]] constexpr auto SampleRecord::max_num_samples() -> usize {
    return max_samples;
}


[[nodiscard]] constexpr auto SampleRecord::max_num_samples() const -> usize {
    return max_samples;
}


[[nodiscard]] auto SampleRecord::write_timings_to_csv(const std::string_view path)
    -> Fallible {
    if (samples().empty()) {
        return std::unexpected{
            make_log_string(LogLevel::Warning, "profiler: nothing to write to '{}'", path)
        };
    }

    namespace chrono = std::chrono;

    const auto now = chrono::floor<chrono::seconds>(current_datetime());
    const auto fname = std::format("arcxel_{:%F_%H-%M-%S}.csv", now);
    const auto fpath = std::filesystem::path{ path } / fname;

    // TODO: overwrite warning
    auto file = std::fstream(fpath, std::ios::trunc | std::ios::out);

    if (!file.is_open()) {
        return std::unexpected(make_log_string(
            LogLevel::Error,
            "profiler: could not open {} for writing",
            fname));
    }

    // CSV headings
    std::println(file, "label,depth,thread,start (ns),end (ns),duration (ns)");

    // Sort samples based on Sample::start, directly uses
    // member as std::ranges::sort is in-place.
    std::ranges::sort(samples_store, {}, [](const auto& s) { return s.start; });

    const auto local_epoch = samples().front().start;

    for (const auto& sample : samples()) {
        const auto start = sample.start - local_epoch;
        const auto end = sample.end - local_epoch;
        const auto diff = sample.end - sample.start;

        std::println(
            file,
            "{},{},{},{},{},{}",
            sample.label,
            sample_label_to_depth(sample.label),
            sample.tid,
            as_us(start),
            as_us(end),
            as_us(diff));
    }

    log(LogLevel::Info,
        "profiler: wrote {} samples to {}",
        samples().size(),
        fpath.string());

    return {};
}

} // namespace arcxel
