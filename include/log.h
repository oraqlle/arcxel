// <log.h> -*- C++ -*-

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

#include <conf.h>
#include <ostream>
#include <ranges>
#include <types.h>

#include <chrono>
#include <format>
#include <fstream>
#include <syncstream>
#include <utility>

namespace arcxel::log {

enum class LogLevel : u8 {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
    Off
}; // enum class Level

// turn into std::formatter()
[[nodiscard]] auto to_string(LogLevel level) noexcept -> const char*;

#ifdef ARCXEL_LOGGING
    static constexpr bool logging_enabled = true;
    static constexpr LogLevel min_log_level = debug_enabled ? LogLevel::Trace : LogLevel::Info;
#else
    static constexpr bool logging_enabled = false
    static constexpr Level min_log_level = Level::Off
#endif

/**
 * @brief Callback for raylibs internal logs
 */
auto raylib_log_callback(int raylib_level, const char* text, va_list args) -> void;

/**
 * @brief Capture and redirect raylib's trace logs to our own logger.
 * Must be called before InitWindow so raylib's start-up messages are captured too.
 */
auto capture_raylib_logs() -> void;


class Logger {
private:
    Logger() noexcept;

public:
    static auto instance() -> Logger& {
        static auto logger = Logger();
        return logger;
    }

    template <typename... Args>
    auto log(const LogLevel level, std::format_string<Args...> fmt, Args&&... args) -> void {
        if (level >= min_log_level) {
            const auto now = std::chrono::system_clock::now();
            const auto second = std::chrono::floor<std::chrono::seconds>(now);
            const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - second).count();

            print(os, "[{:%H:%M:%S}{:03}] {:<5} ", now, millis, to_string(level));
            println(os, fmt, std::forward<Args...>(args)...);
            std::flush(os);
        }
    }

private:
    std::fstream file;
    std::osyncstream os;
}; // class Logger

} // namespace arcxel::log
