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

#include <format>
#include <string_view>
#include <utility>

// The build system supplies this; the fallback is for compiling without CMake.
#ifndef ARCXEL_LOGGING
#    define ARCXEL_LOGGING 1
#endif

namespace arcxel::log {

// Ordered by increasing severity; comparisons rely on that order.
enum class Level {
    trace,
    debug,
    info,
    warn,
    error,
    fatal,
    off,
};

inline constexpr bool logging_enabled = ARCXEL_LOGGING != 0;

#ifdef NDEBUG
inline constexpr bool debug_build = false;
#else
inline constexpr bool debug_build = true;
#endif

// Release drops trace and debug, so they cost nothing in a measured build.
inline constexpr Level build_level = debug_build ? Level::trace : Level::info;

// The lowest level this build keeps. Calls below it are discarded at compile
// time, and ARCXEL_LOGGING set OFF discards every level.
inline constexpr Level min_level = logging_enabled ? build_level : Level::off;

[[nodiscard]] auto to_string(Level level) noexcept -> const char*;

auto set_level(Level level) noexcept -> void;
[[nodiscard]] auto level() noexcept -> Level;

// Applies ARCXEL_LOG_LEVEL if set: trace, debug, info, warn, error, fatal or off
// It can only raise the threshold, never lower it past min_level
auto set_level_from_env() -> void;

// Redirect raylib's own TraceLog output through this logger. Call before
// InitWindow so raylib's start-up messages are captured too.
auto adopt_raylib() noexcept -> void;

// Mirror output to a file as well as stderr, replacing any file already open.
// Lines are flushed as they are written so a crash still leaves a usable log.
[[nodiscard]] auto set_file(std::string_view path) -> bool;
auto close_file() noexcept -> void;

namespace detail {

auto write(Level level, std::string_view message) -> void;

template <Level L>
inline constexpr bool compiled_in = L >= min_level;

// The gate every level goes through. The compile-time check comes first, so a
// level this build discards generates nothing at all. Named dispatch rather than
// emit: log.cxx already has an emit() that writes a finished line to the sinks.
template <Level L, typename... Args>
auto dispatch(std::format_string<Args...> fmt, Args&&... args) -> void {
    if constexpr (compiled_in<L>) {
        if (L >= level()) {
            write(L, std::format(fmt, std::forward<Args>(args)...));
        }
    }
}

} // namespace detail

template <typename... Args>
auto trace(std::format_string<Args...> fmt, Args&&... args) -> void {
    detail::dispatch<Level::trace>(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
auto debug(std::format_string<Args...> fmt, Args&&... args) -> void {
    detail::dispatch<Level::debug>(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
auto info(std::format_string<Args...> fmt, Args&&... args) -> void {
    detail::dispatch<Level::info>(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
auto warn(std::format_string<Args...> fmt, Args&&... args) -> void {
    detail::dispatch<Level::warn>(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
auto error(std::format_string<Args...> fmt, Args&&... args) -> void {
    detail::dispatch<Level::error>(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
auto fatal(std::format_string<Args...> fmt, Args&&... args) -> void {
    detail::dispatch<Level::fatal>(fmt, std::forward<Args>(args)...);
}

} // namespace arcxel::log
