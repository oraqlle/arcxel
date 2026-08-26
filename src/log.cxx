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

#include <cstdarg>
#include <cstdio>
#include <optional>
#include <format>
#include <string>
#include <string_view>

namespace arcxel::log {

auto parse_level(std::string_view name) noexcept -> std::optional<LogLevel> {
    if (name == "trace") { return LogLevel::Trace; }
    if (name == "debug") { return LogLevel::Debug; }
    if (name == "info") { return LogLevel::Info; }
    if (name == "warn") { return LogLevel::Warning; }
    if (name == "error") { return LogLevel::Error; }
    if (name == "fatal") { return LogLevel::Fatal; }
    if (name == "off") { return LogLevel::Off; }

    return std::nullopt;
}

[[nodiscard]] auto to_string(LogLevel lvl) noexcept -> const char* {
    switch (lvl) {
    case LogLevel::Trace: return "TRACE";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info: return "INFO";
    case LogLevel::Warning: return "WARNING";
    case LogLevel::Error: return "ERROR";
    case LogLevel::Fatal: return "FATAL";
    case LogLevel::Off: return "OFF";
    default: return "<unknown>";
    }
}

/**
 * Intnernal function to convert raylib log enum to Arcxel's
 */
static auto from_raylib_log_level(int raylib_level) noexcept -> LogLevel {
    switch (raylib_level) {
    case LOG_TRACE: return LogLevel::Trace;
    case LOG_DEBUG: return LogLevel::Debug;
    case LOG_INFO: return LogLevel::Info;
    case LOG_WARNING: return LogLevel::Warning;
    case LOG_ERROR: return LogLevel::Error;
    case LOG_FATAL: return LogLevel::Fatal;
    default: return LogLevel::Info;
    }
}

auto raylib_log_callback(int raylib_level, const char* text, va_list args) -> void {
    const auto size = std::vsnprintf(nullptr, 0, text, args);

    if (size < 0) {
        return;
    }

    auto buf = std::string(size, '\0');
    vsnprintf(buf.data(), size, text, args);

    const auto level = from_raylib_log_level(raylib_level);
    Logger::instance().log(level, "{}", buf);
}

auto capture_raylib_logs() -> void {
    if constexpr (logging_enabled) {
        SetTraceLogLevel(LOG_ALL);
        SetTraceLogCallback(raylib_log_callback);
    } else {
        SetTraceLogLevel(LOG_NONE);
        SetTraceLogCallback(raylib_log_callback);
    }
}

} // namespace arcxel::log
