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
#include <iostream>
#include <print>
#include <syncstream>
#include <filesystem>
#include <format>
#include <utility>

namespace arcxel::log {

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

} // namespace arcxel::log


namespace std {

template<>
struct std::formatter<arcxel::log::LogLevel> {

    inline constexpr auto _M_to_string(arcxel::log::LogLevel level) const
        -> std::string_view {
        switch (level) {
            case arcxel::log::LogLevel::Trace:
                return "TRACE";
            case arcxel::log::LogLevel::Debug:
                return "DEBUG";
            case arcxel::log::LogLevel::Info:
                return "INFO";
            case arcxel::log::LogLevel::Warning:
                return "WARNING";
            case arcxel::log::LogLevel::Error:
                return "ERROR";
            case arcxel::log::LogLevel::Fatal:
                return "FATAL";
            case arcxel::log::LogLevel::Off:
                return "OFF";
            default:
                return "UNKNOWN";
        }
    }


    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    
    template <typename FormatContext>
    auto format(arcxel::log::LogLevel level, FormatContext& ctx) {
        return std::format_to(ctx.out(), "{:<5}", _M_to_string(level));
    }

}; // struct std::formatter<arcxel::log::LogLevel>

} // namespace std


namespace arcxel::log {

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


// TODO: make_log_msg() -> constructs log string


// TODO: log_to() -> constructs log string


/**
 * @brief Logs a formatted string message to stderr and optionally a file if
 * setup_file_logging() been called.
 */
template <typename... Args>
auto log(const LogLevel level, std::format_string<Args...> fmt, Args&&... args) -> void {
    if constexpr (logging_enabled) {
        if (level >= min_log_level) {
            const auto now = std::chrono::system_clock::now();
            const auto second = std::chrono::floor<std::chrono::seconds>(now);
            const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - second).count();

            const auto log_details = format("[{:%H:%M:%S}{:03}] {} ", now, millis, level);
            const auto msg = format(fmt, std::forward<Args...>(args)...);

            if (logfile.is_open()) {
                auto synclog = std::osyncstream{logstream};
                std::print(synclog, log_details);
                std::println(synclog, msg);
                std::flush(synclog);
            }

            std::print(syncederr, log_details);
            std::println(syncederr, msg);
            std::flush(syncederr);
        }

    }
}

} // namespace arcxel::log


