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

} // namespace arcxel
