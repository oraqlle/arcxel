// <single_item_channel.h> -*- C++ -*-

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

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

namespace arcxel {

template <typename T>
class SingleItemChannel {
public:
    SingleItemChannel() = default;

    SingleItemChannel(const SingleItemChannel&) = delete;
    SingleItemChannel& operator=(const SingleItemChannel&) = delete;


    auto push(T value) -> void {
        {
            auto lock = std::unique_lock(mtex);
            not_full.wait(lock, [this] { return !item.has_value() || closed; });

            if (closed) {
                return;
            }

            item.emplace(std::move(value));
        }

        not_empty.notify_one();
    }


    [[nodiscard]] auto pop(T& value) -> bool {
        {
            auto lock = std::unique_lock(mtex);

            not_empty.wait(lock, [this] { return item.has_value() || closed; });

            if (!item.has_value()) {
                return false;
            }


            value = std::move(*item);
            item.reset();
        }

        not_full.notify_one();
        return true;
    }


    auto close() -> void {
        {
            auto lock = std::lock_guard(mtex);
            closed = true;
        }

        not_empty.notify_all();
        not_full.notify_all();
    }

private:
    std::mutex mtex;
    std::condition_variable not_empty;
    std::condition_variable not_full;
    std::optional<T> item = std::nullopt;
    bool closed = false;
}; // class SingleItemChannel

} // namespace arcxel