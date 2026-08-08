// <log/logger.cxx> -*- C++ -*-

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

#include <log/logger.h>

#include <log/level.h>

#include <raylib.h>

#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <format>
#include <string_view>

namespace arcxel::log {

namespace {

std::atomic<Level> current_level{Level::info};

// One write per line, so lines from different threads cannot interleave
// mid-line. std::print would need GCC 14 or MSVC 19.37; std::format is C++20.
auto emit(std::string_view line) noexcept -> void {
    std::fwrite(line.data(), 1, line.size(), stderr);
}

// Thread safe, unlike std::localtime which shares one static tm.
auto to_local(std::time_t stamp, std::tm& out) noexcept -> bool {
#ifdef _WIN32
    return ::localtime_s(&out, &stamp) == 0;
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

// raylib supplies a printf-style format and a va_list, so the message has to be
// rendered here before it can join the std::format path. Nothing may escape:
// this is called from C.
auto raylib_callback(int raylib_level, const char* text, va_list args) noexcept
    -> void {
    try {
        char buffer[1024];

        if (std::vsnprintf(buffer, sizeof(buffer), text, args) < 0) {
            return;
        }

        const auto lvl = from_raylib(raylib_level);

        if (lvl >= level()) {
            detail::write(lvl, std::string_view(buffer));
        }
    } catch (...) {
    }
}

} // namespace

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

auto set_level(Level lvl) noexcept -> void {
    current_level.store(lvl, std::memory_order_relaxed);
}

auto level() noexcept -> Level {
    return current_level.load(std::memory_order_relaxed);
}

auto adopt_raylib() noexcept -> void {
    // Let everything through raylib's own filter; ours decides what is kept.
    SetTraceLogLevel(LOG_ALL);
    SetTraceLogCallback(raylib_callback);
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
