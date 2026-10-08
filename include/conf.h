// <conf.h> -*- C++ -*-

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

#include "types.h"
#include "game_object.h"

#include <string>
#include <string_view>
#include <thread>
#include <memory>
#include <vector>

namespace arcxel {

#if ARCXEL_DEBUG
static constexpr bool debug_enabled = true;
#else
static constexpr bool debug_enabled = false;
#endif

#if ARCXEL_PHYSICS_DEBUG_RENDERER
static constexpr bool physics_debug_renderer_enabled = true;
static constexpr usize default_num_sim_objects = 50;
#else
static constexpr bool physics_debug_renderer_enabled = false;
static constexpr usize default_num_sim_objects = 1000;
#endif


enum class ThreadingType : u8 { Serial, Broad, Fine };

#if ARCXEL_THREADING_TASK_BASED
static constexpr ThreadingType threading_model = ThreadingType::Broad;
static constexpr u8 min_threads_required = 3;

using ObjectsArray = std::vector<std::unique_ptr<GameObject>>;

struct FramePacket {
    ObjectsArray objects;
    f64 delta = 0.0;
}; // struct FramePacket

#elif ARCXEL_THREADING_FINE
static constexpr ThreadingType threading_model = ThreadingType::Fine;
static constexpr u8 min_threads_required = 2;
#else
static constexpr ThreadingType threading_model = ThreadingType::Serial;
static constexpr u8 min_threads_required = 1;
#endif


constexpr std::string_view DEFAULT_TRACES_DIR = "traces";
constexpr std::string_view DEFAULT_LOGS_DIR = "logs";


struct Config {
    const u32 num_hw_threads = std::thread::hardware_concurrency();

    u32 num_available_threads = num_hw_threads;
    u32 num_required_threads = min_threads_required;

    usize num_sim_objects = arcxel::default_num_sim_objects;

    std::string trace_dir = static_cast<std::string>(DEFAULT_TRACES_DIR);
    std::string log_dir = static_cast<std::string>(DEFAULT_LOGS_DIR);

    std::string window_name = "Arcxel Window";
}; // struct Config

} // namespace arcxel
