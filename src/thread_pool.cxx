// <thread_pool.cxx> -*- C++ -*-

#include "thread_pool.h"
#include "types.h"

#include <algorithm>
#include <mutex>
#include <queue>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>

namespace arcxel {

ThreadPool::ThreadPool(std::optional<usize> num_threads_opt) noexcept
    : num_outstanding(0)
    , stopping(false) {
    const auto requested = num_threads_opt.value_or(std::thread::hardware_concurrency());
    const auto count = std::max(usize{ 1 }, requested);

    workers.reserve(count);

    // start worker threads
    for ([[maybe_unused]] auto i : std::views::iota(usize{ 0 }, count)) {
        workers.emplace_back([this] { _M_worker(); });
    }
}


ThreadPool::~ThreadPool() noexcept {
    {
        const auto lock = std::lock_guard(mtx);
        stopping = true;
    }

    queued.notify_all();

    for (auto& worker : workers) {
        worker.join();
    }
}


[[nodiscard]] auto ThreadPool::singleton(std::optional<usize> num_threads)
    -> ThreadPool& {
    static auto pool = ThreadPool(std::move(num_threads));
    return pool;
}

// submit task to the queue and wakes a worker thread
auto ThreadPool::submit(Task task) -> void {
    {

        const auto lock = std::lock_guard(mtx);
        tasks.push(std::move(task));
        num_outstanding += 1;
    }

    queued.notify_one();
}


auto ThreadPool::wait() -> void {
    auto lock = std::unique_lock(mtx);
    drained.wait(lock, [this] { return num_outstanding == 0; });
}


[[nodiscard]] auto ThreadPool::size() const -> usize { return workers.size(); }


auto ThreadPool::_M_worker() -> void {
    while (true) {

        auto task = Task{};

        {
            auto lock = std::unique_lock(mtx);
            queued.wait(lock, [this] { return stopping || !tasks.empty(); });

            if (stopping && tasks.empty()) {
                return;
            }

            // take on task
            task = std::move(tasks.front());
            tasks.pop();
        }

        task();

        auto finished_last = false;

        {
            const auto lock = std::lock_guard(mtx);
            num_outstanding -= 1;
            finished_last = num_outstanding == 0;
        }

        if (finished_last) {
            drained.notify_all();
        }
    }
}

} // namespace arcxel
