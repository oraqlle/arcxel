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
#include <types.h>

#include <chrono>
#include <format>
#include <fstream>
#include <string_view>
#include <utility>

namespace arcxel::log {

enum class Level : u8 {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
    Off
}; // enum class Level

// turn into std::formatter()
[[nodiscard]] auto to_string(Level level) noexcept -> const char*;

#ifdef ARCXEL_LOGGING
    static constexpr bool logging_enabled = true;
    static constexpr Level min_log_level = debug_enabled ? Level::Trace : Level::Info;
#else
    static constexpr bool logging_enabled = false
    static constexpr Level min_log_level = Level::Off
#endif

/**
 * @brief Capture and redirect raylib's trace logs to our own logger.
 * Must be called before InitWindow so raylib's start-up messages are captured too.
 */
auto capture_raylib_logs() -> void;

class Logger {
public:
    Logger() noexcept;

    template <Level L, typename... Args>
    auto log(std::format_string<Args...> fmt, Args&&... args) -> void {
        if constexpr (L >= min_log_level) {

            const auto now = std::chrono::system_clock::now();
            const auto second = std::chrono::floor<std::chrono::seconds>(now);
            const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - second).count();

            auto local = std::tm{};

            if (!to_local(std::chrono::system_clock::to_time_t(second), local)) {
                file << std::format("[--:--:--.---] {:<5} {}\n", to_string(lvl), message);
                return;
            }

            file << std::format(
                "[{:02}:{:02}:{:02}.{:03}] {:<5} {}\n", local.tm_hour, local.tm_min,
                local.tm_sec, millis, to_string(lvl), message
            );

            std::print(file, fmt, std::forward(args)...);
        }
    }

private:
    std::fstream file;
}; // class Logger

} // namespace arcxel::log
