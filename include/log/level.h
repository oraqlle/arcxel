// <log/level.h> -*- C++ -*-

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

#ifndef ARCXEL_LOG_LEVEL_H
#define ARCXEL_LOG_LEVEL_H

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

} // namespace arcxel::log

#endif // ARCXEL_LOG_LEVEL_H
