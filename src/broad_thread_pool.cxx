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

// constructor
BroadThreadPool::BroadThreadPool(std::optional<usize> num_threads) noexcept
    : outstanding(0)
    , stopping(false) {
        const auto requested = num_threads.value_or(std::thread::hardware_concurrency());
        const auto count = std::max(usize{1}, requested);

        workers.reserve(count);

        // start worker threads
        for ([[maybe_unused]] auto i : std::views::iota(usize{0}, count)) {
            workers.emplace_back([this] { _M_worker();});
        }
    }

// destructor
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

// shared state
[[nodiscard]] auto BroadThreadPool::singleton(std::optional<usize> init)
    -> BroadThreadPool& {
        static auto pool = BroadThreadPool(std::move(init));
        return pool;
    }

// submit task to the queue and wakes a worker thread
auto BroadThreadPool::submit(Task task) -> void {
    {
        
        const auto lock = std::lock_guard(mtx);
        tasks.push(std::move(task));
        outstanding += 1;
    }

    queued.notify_one();
}


// blocks main thread until all submitted tasks are finished
auto BroadThreadPool::wait() -> void {
    //sleeping
    auto lock = std::unique_lock(mtx);
    drained.wait(lock, [this]  {
        return outstanding == 0;
    });
}

// num of threads
[[nodiscard]] auto BroadThreadPool::size() const -> usize {
    return workers.size();
}

// individual work threads
auto BroadThreadPool::_M_worker() -> void {
    // wait for a task
    while (true) {
        auto task = Task{};
        {
            //sleep until theres work
            auto lock = std::unique_lock(mtx);
            queued.wait(lock, [this] {
                return stopping || !tasks.empty();
            });

            //continue picking tasks up until there are no more in queue
            if (stopping && tasks.empty()) {
                return;
            }

            // take on task
            task = std::move(tasks.front());
            tasks.pop();

        } // release mutex
    
    // run task
    task();
    auto finished_last = false;

    // task is completed
    {
        const auto lock = std::lock_guard(mtx);
        outstanding -= 1;
        finished_last = outstanding == 0;
    }

    // notify main thread that worker is free
    // every task is finished
    if (finished_last) {
        drained.notify_all();
    }
}

}

} // namespace arcxel