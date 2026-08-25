// <log.cxx> -*- C++ -*-

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

#include "log.h"

#include <raylib.h>

#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <ctime>
#include <format>
#include <string>
#include <string_view>

namespace arcxel::log {

namespace {

std::atomic<Level> current_level{Level::info};

std::FILE* sink = nullptr;

auto to_string(Level lvl) noexcept -> const char* {
    switch (lvl) {
    case Level::trace: return "TRACE";
    case Level::debug: return "DEBUG";
    case Level::info: return "INFO";
    case Level::warn: return "WARN";
    case Level::error: return "ERROR";
    case Level::fatal: return "FATAL";
    case Level::off: return "OFF";
    }

    return "?";
}

auto level_from_name(std::string_view name) noexcept -> std::optional<Level> {
    if (name == "trace") { return Level::trace; }
    if (name == "debug") { return Level::debug; }
    if (name == "info") { return Level::info; }
    if (name == "warn") { return Level::warn; }
    if (name == "error") { return Level::error; }
    if (name == "fatal") { return Level::fatal; }
    if (name == "off") { return Level::off; }

    return std::nullopt;
}

// One write per line, so lines from different threads cannot interleave
// mid-line. std::print would need GCC 14 or MSVC 19.37; std::format is C++20.
auto emit(std::string_view line) noexcept -> void {
    std::fwrite(line.data(), 1, line.size(), stderr);

    if (sink != nullptr) {
        std::fwrite(line.data(), 1, line.size(), sink);
        std::fflush(sink);
    }
}

// Thread safe, unlike std::localtime which shares one static tm.
auto to_local(std::time_t stamp, std::tm& out) noexcept -> bool {
#if defined(_MSC_VER)
    return ::localtime_s(&out, &stamp) == 0;
#elif defined(_WIN32)
    // MinGW defines _WIN32 but not the MSVC localtime_s. The Windows CRT keeps
    // localtime's result in thread-local storage, so copying it out is safe.
    const auto* result = std::localtime(&stamp);

    if (result == nullptr) {
        return false;
    }

    out = *result;
    return true;
#else
    return ::localtime_r(&stamp, &out) != nullptr;
#endif
}

auto from_raylib(int raylib_level) noexcept -> Level {
    switch (raylib_level) {
    case LOG_TRACE: return Level::trace;
    case LOG_DEBUG: return Level::debug;
    case LOG_INFO: return Level::info;
    case LOG_WARNING: return Level::warn;
    case LOG_ERROR: return Level::error;
    case LOG_FATAL: return Level::fatal;
    default: return Level::info;
    }
}

[[maybe_unused]] auto raylib_callback(
    int raylib_level, const char* text, va_list args
) noexcept -> void {
    char buffer[1024];

    if (std::vsnprintf(buffer, sizeof(buffer), text, args) < 0) {
        return;
    }

    const auto lvl = from_raylib(raylib_level);

    if (lvl >= level()) {
        detail::write(lvl, std::string_view(buffer));
    }
}

auto set_level(Level lvl) noexcept -> void {
    current_level.store(lvl, std::memory_order_relaxed);
}

} // namespace

[[nodiscard]] auto level() noexcept -> Level {
    return current_level.load(std::memory_order_relaxed);
}

auto set_level_from_env() -> void {
    const auto* value = std::getenv("ARCXEL_LOG_LEVEL");

    if (value == nullptr) {
        return;
    }

    const auto parsed = level_from_name(value);

    if (!parsed.has_value()) {
        warn("log: ignoring unrecognised ARCXEL_LOG_LEVEL '{}'", value);
        return;
    }

    // Say so before switching, or raising the threshold would hide the notice.
    info("log: level set to {} by ARCXEL_LOG_LEVEL", to_string(*parsed));

    if (*parsed < min_level) {
        warn(
            "log: this build discards anything below {}, so some messages stay "
            "unavailable",
            to_string(min_level)
        );
    }

    set_level(*parsed);
}

[[nodiscard]] auto set_file(std::string_view path) -> bool {
    close_file();

    const auto name = std::string(path);
    sink = std::fopen(name.c_str(), "w");

    if (sink == nullptr) {
        error("log: could not open {} for writing", name);
        return false;
    }

    info("log: also writing to {}", name);
    return true;
}

auto close_file() noexcept -> void {
    if (sink == nullptr) {
        return;
    }

    std::fclose(sink);
    sink = nullptr;
}

auto adopt_raylib() noexcept -> void {
    // Nothing would be written when logging is off, so raylib keeps its own
    // output rather than routing it through a logger that discards everything.
    if constexpr (logging_enabled) {
        // Let everything through raylib's own filter; ours decides what is kept.
        SetTraceLogLevel(LOG_ALL);
        SetTraceLogCallback(raylib_callback);
    }
}

namespace detail {

auto write(Level lvl, std::string_view message) -> void {
    const auto now = std::chrono::system_clock::now();
    const auto second = std::chrono::floor<std::chrono::seconds>(now);
    const auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - second).count();

    auto local = std::tm{};

    if (!to_local(std::chrono::system_clock::to_time_t(second), local)) {
        emit(std::format("[--:--:--.---] {:<5} {}\n", to_string(lvl), message));
        return;
    }

    emit(std::format(
        "[{:02}:{:02}:{:02}.{:03}] {:<5} {}\n", local.tm_hour, local.tm_min,
        local.tm_sec, millis, to_string(lvl), message
    ));
}

} // namespace detail

} // namespace arcxel::log
