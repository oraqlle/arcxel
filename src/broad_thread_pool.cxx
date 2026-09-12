// <broad_thread_pool.cxx> -*- C++ -*-

#include "broad_thread_pool.h"
#include "types.h"

#include <algorithm>
#include <mutex>
#include <queue>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>

namespace arcxel {

BroadThreadPool::BroadThreadPool(std::optional<usize> num_threads) noexcept
    : outstanding(0)
    , stopping(false) {
        const auto requested = num_threads.value_or(std::thread::hardware_concurrency());
        const auto count = std::max(usize{1}, requested);

        workers.reserve(count);

        for ([[maybe_unused]] auto i : std::views::iota(usize{0}, count)) {
            workers.emplace_back([this] { _M_worker();});
        }
    }

BroadThreadPool::~BroadThreadPool() noexcept {
    {
        const auto lock = std::lock_guard(mtx);
        stopping = true;
    }

    queued.notify_all();

    for (auto& worker : workers) {
        worker.join();
    }
}

[[nodiscard]] auto BroadThreadPool::singleton(std::optional<usize> init)
    -> BroadThreadPool& {
        static auto pool = BroadThreadPool(std::move(init));
        return pool;
    }

auto BroadThreadPool::submit(Task task) -> void {
    {
        const auto lock = std::lock_guard(mtx);
        tasks.push(std::move(task));
        outstanding += 1;
    }

    queued.notify_one();
}

auto BroadThreadPool::wait() -> void {
    auto lock = std::unique_lock(mtx);
    drained.wait(lock, [this]  {
        return outstanding == 0;
    });
}

[[nodiscard]] auto BroadThreadPool::size() const -> usize {
    return workers.size();
}

auto BroadThreadPool::_M_worker() -> void {
    while (true) {
        auto task = Task{};
        {
            auto lock = std::unique_lock(mtx);
            queued.wait(lock, [this] {
                return stopping || !tasks.empty();
            });

            //drain whatever is queued before leaving
            if (stopping && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

    task();
    auto finished_last = false;

    {
        const auto lock = std::lock_guard(mtx);
        outstanding -= 1;
        finished_last = outstanding == 0;
    }

    if (finished_last) {
        drained.notify_all();
    }
}

}

} // namespace arcxel