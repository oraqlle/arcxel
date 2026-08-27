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
#include <raylib.h>
#include <types.h>

#include <chrono>
#include <iostream>
#include <fstream>
#include <print>
#include <syncstream>
#include <filesystem>
#include <format>
#include <utility>

namespace arcxel {

extern std::fstream logfile;
extern std::iostream logstream;
extern std::osyncstream syncederr;


enum class LogLevel : u8 {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
    Off
}; // enum class LogLevel


inline constexpr auto loglevel_to_string(arcxel::LogLevel level) -> std::string_view {
    switch (level) {
        case arcxel::LogLevel::Trace:
            return "TRACE";
        case arcxel::LogLevel::Debug:
            return "DEBUG";
        case arcxel::LogLevel::Info:
            return "INFO";
        case arcxel::LogLevel::Warning:
            return "WARNING";
        case arcxel::LogLevel::Error:
            return "ERROR";
        case arcxel::LogLevel::Fatal:
            return "FATAL";
        case arcxel::LogLevel::Off:
            return "OFF";
        default:
            return "UNKNOWN";
    }
}

} // namespace arcxel


template <>
struct std::formatter<arcxel::LogLevel, char>
    : public std::formatter<std::string_view, char> {

    using formatter_t = std::formatter<std::string_view, char>;


    template <typename FormatContext>
    auto format(arcxel::LogLevel level, FormatContext& ctx) {
        return formatter_t::format(loglevel_to_string(level), ctx);
    }

}; // struct std::formatter<arcxel::LogLevel, char>


namespace arcxel {

#ifdef ARCXEL_LOGGING
    static inline constexpr bool logging_enabled = true;
    static inline constexpr LogLevel min_log_level = debug_enabled ? LogLevel::Trace : LogLevel::Info;
#else
    static inline constexpr bool logging_enabled = false
    static inline constexpr Level min_log_level = Level::Off
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


/**
 * @brief Open file to be used for trace logging
 */
[[nodiscard]] auto open_log_file(const std::filesystem::path& outdir) -> Fallible;


/**
 * @brief Close file used for trace logging
 */
[[nodiscard]] auto close_log_file() -> Fallible;


/**
 * @brief Constructs a log message from the given LogLevel and the format string
 */
template <typename... Args>
constexpr auto
make_log_string(const LogLevel level, std::format_string<Args...> fmt, Args&&... args)
    -> std::string {
    const auto now = std::chrono::system_clock::now();
    const auto second = std::chrono::floor<std::chrono::seconds>(now);
    const auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - second).count();

    return std::format(
        "[{:%H:%M:%S}{:03}] {:<5} {}",
        now,
        millis,
        loglevel_to_string(level),
        std::format(fmt, std::forward<Args>(args)...)
    );
}


/**
 * @brief Logs a formatted string to the designated stream object
 */
template <typename Stream, typename... Args>
auto log_to(
    Stream& stream, const LogLevel level, std::format_string<Args...> fmt, Args&&... args
) -> void {
    if constexpr (logging_enabled) {
        if (level >= min_log_level) {
            auto msg = make_log_string(level, fmt, std::forward<Args>(args)...);

            std::println(stream, msg);
            std::flush(stream);
        }
    }
}

/**
 * @brief Logs a formatted string message to stderr and optionally a file if
 * setup_file_logging() been called.
 */
template <typename... Args>
auto log(const LogLevel level, std::format_string<Args...> fmt, Args&&... args) -> void {
    if constexpr (logging_enabled) {
        if (level >= min_log_level) {
            auto msg = make_log_string(level, fmt, std::forward<Args>(args)...);

            if (logfile.is_open()) {
                auto synclog = std::osyncstream{logstream};
                std::println(synclog, "{}", msg);
                std::flush(synclog);
            }

            std::println(syncederr, "{}", msg);
            std::flush(syncederr);
        }
    }
}

} // namespace arcxel

