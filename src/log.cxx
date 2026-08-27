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
#include "types.h"

#include <istream>
#include <raylib.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <print>
#include <cstdarg>
#include <cstdio>
#include <ios>
#include <optional>
#include <filesystem>
#include <format>
#include <sstream>
#include <streambuf>
#include <string>
#include <string_view>
#include <syncstream>

namespace arcxel::log {


//static bool file_logging_enabled = false;
std::fstream logfile = std::fstream();
std::iostream logstream = std::iostream(nullptr); //< /dev/null by default (no-op)
std::osyncstream syncederr = std::osyncstream(std::cerr);


/**
 * @brief Helper to change buffer used by a std::iostream object
 */
static auto change_rdbuf_to(std::iostream& io, std::streambuf* rdbuf) -> std::streambuf* {
    auto* old = io.rdbuf(rdbuf);
    io.clear();
    return old;
}


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
    default: return "UNKNOWN";
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
    log(level, "{}", buf);
}


auto capture_raylib_logs() -> void {
    SetTraceLogLevel(LOG_ALL);
    SetTraceLogCallback(raylib_log_callback);
}


auto open_log_file(const std::filesystem::path& outdir) -> Fallible {
    // Check if log file already open
    // close and set up new file.
    if (logfile.is_open()) {
        std::flush(logfile);
        logfile.clear();
        logfile.close();
    }

    namespace fs = std::filesystem;

    // Temporary stringstream to capture logs before file construction
    auto ss = std::stringstream{};
    auto* old_rdbuf = change_rdbuf_to(logstream, ss.rdbuf());

    // Construct filename from current date and time
    const auto now = std::chrono::system_clock::now();
    const auto datetime = std::chrono::floor<std::chrono::days>(now);
    const auto fname = std::format("{:%Y-%m-%d_%H:%M:%S}.log", datetime);
    const auto fpath = outdir / fname;

    // Check if filesystem object of the same name exists
    if (!fs::exists(fpath)) {
        log(LogLevel::Info, "Creating '{}' exists, overwriting.", fpath.filename().string());
    } else if (fs::status(outdir).type() != fs::file_type::regular) {
        return std::unexpected(std::format("File '{}' exists, overwriting.", fpath.filename().string()));
    } else {
        log(LogLevel::Warning, "File '{}' exists, overwriting.", fpath.filename().string());
    }

    // Open fstream object for logging
    logfile.open(fpath, std::ios::out);

    if (logfile.is_open()) {
        change_rdbuf_to(logstream, logfile.rdbuf());

        // dump stringstream logs
        std::print(logstream, "{}", ss.str());

        log(LogLevel::Info, "Successfully opened log file '{}'", fpath.string());
    } else {
        change_rdbuf_to(logstream, old_rdbuf);

        logfile.clear();

        log(LogLevel::Error, "Could not open file '{}'.", fpath.string());
        log(LogLevel::Info, "Logs only outputting to STDERR");
    }

    return {};
}

[[nodiscard]] auto close_log_file() -> Fallible {
    if (!logfile.is_open()) {
        return std::unexpected("Attempting to close file stream that is unopen");
    }

    log(LogLevel::Info, "Closing and saving log file");

    change_rdbuf_to(logstream, nullptr);
    logfile.close();

    return {};
}

} // namespace arcxel::log
