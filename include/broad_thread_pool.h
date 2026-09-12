// <broad_thread_pool.h> -*- C++ -*-

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

#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

namespace arcxel {

    class BroadThreadPool {
    public:
        using Task = std::function<void()>;

        [[nodiscard]] static auto singleton(std::optional<usize> init = std::nullopt) 
            -> BroadThreadPool&;

        auto submit(Task task) -> void; // queue the task
        auto wait() -> void;            // main thread only
        [[nodiscard]] auto size() const -> usize;

    private:
        explicit BroadThreadPool(std::optional<usize> num_threads) noexcept;
        ~BroadThreadPool() noexcept;
        auto _M_worker() -> void;

    private:
        std::vector<std::thread> workers;
        std::queue<Task> tasks;
        std::mutex mtx;
        std::condition_variable queued;     // workers sleep here
        std::condition_variable drained;    // wait() sleeps here

        usize outstanding;                  // submitted tasks but not finished tasks
        bool stopping;
        
    }; // class BroadThreadPool

} // namespace arcxel