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
#include <functional>
#include <string>
#include <string_view>

namespace arcxel::log {

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

[[nodiscard]] auto to_string(Level lvl) noexcept -> const char* {
    switch (lvl) {
    case Level::Trace: return "TRACE";
    case Level::Debug: return "DEBUG";
    case Level::Info: return "INFO";
    case Level::Warning: return "WARNING";
    case Level::Error: return "ERROR";
    case Level::Fatal: return "FATAL";
    case Level::Off: return "OFF";
    default: return "<unknown>";
    }
}

auto capture_raylib_logs() noexcept -> void {
    if constexpr (logging_enabled) {
        SetTraceLogLevel(LOG_ALL);
        SetTraceLogCallback(raylib_log_callback);
    } else {
        SetTraceLogLevel(LOG_NONE);
        SetTraceLogCallback(raylib_log_callback);
    }
}

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
    case LOG_TRACE: return Level::Trace;
    case LOG_DEBUG: return Level::Debug;
    case LOG_INFO: return Level::Info;
    case LOG_WARNING: return Level::Warning;
    case LOG_ERROR: return Level::Error;
    case LOG_FATAL: return Level::Fatal;
    default: return Level::Info;
    }
}

auto raylib_log_callback(int raylib_level, const char* text, va_list args) -> void {
    char buffer[1024];

    const auto size = std::vsnprintf(buffer, 0, text, args);

    if (size < 0) {
        return;
    }

    auto buf = std::string(size, '\0');
    vsnprintf(buf.data(), size, text, args);

    const auto level = from_raylib(raylib_level);
    logger.log<level>(buf);
}


} // namespace arcxel::log
