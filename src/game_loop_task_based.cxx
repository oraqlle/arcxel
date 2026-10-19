#include "game_loop.h"
#include "conf.h"
#include "single_item_channel.h"
#include "game_object.h"
#include "physics_object.h"
#include "engine.h"
#include "physics.h"
#include "thread_pool.h"
#include "timing.h"
#include "types.h"
#include "log.h"

#include <raylib.h>

#include <stop_token>
#include <thread>
#include <queue>
#include <ranges>
#include <vector>

namespace arcxel {

using Label = Sample::Label;

template<typename T>
class Queue {
public:
    void push(T value) {
        {
            std::lock_guard lock(mutex_);
            queue_.push(std::move(value));
        }
        ready_.notify_one();
    }

    // Blocks until an item is available or the queue is closed.
    std::optional<T> pop() {
        std::unique_lock lock(mutex_);
        ready_.wait(lock, [&] { return closed_ || !queue_.empty(); });

        if (queue_.empty())
            return std::nullopt;

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    void close() {
        {
            std::lock_guard lock(mutex_);
            closed_ = true;
        }
        ready_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable ready_;
    std::queue<T> queue_;
    bool closed_ = false;
}; // class Queue


auto game_loop([[maybe_unused]] Config config, SampleRecord& global_sample_record) -> void {

    if (config.num_available_threads < config.num_required_threads) {
        log(LogLevel::Info, "Starting [SERIAL] game loop [FALLBACK OF TASK_BASED]");
        return serial_game_loop_fallback(config, global_sample_record);
    }

    log(LogLevel::Info, "Starting [TASK_BASED] game loop");

	auto& pool = ThreadPool::singleton();
	auto& scene = Engine::singleton().get_scene();

    while (Engine::singleton().is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, global_sample_record);
        const f64 delta = GetFrameTime();

		{
			const auto _ = Timespan(Label::PhysicsUpdate, global_sample_record);
			Physics::singleton().update(delta);
		}


        {
            const auto _ = Timespan(Label::Update, global_sample_record);
			auto simulation_queue = Queue<usize>{};

            for ([[maybe_unused]] auto i : std::views::iota(usize{ 0 }, pool.size())) {
                pool.submit([&] {
                    while (auto obj_id = simulation_queue.pop()) {
                        if (!obj_id.has_value()) { break; }

                        scene.objects[*obj_id]->update(delta);
                    }
                });
            }

            for (auto idx : std::views::iota(usize{ 0 }, scene.objects.size())) {
                simulation_queue.push(idx);
            }

            simulation_queue.close();
            pool.wait();
        }
		

		{
            const auto _ = Timespan(Label::Render, global_sample_record);
            Engine::singleton().render(delta, global_sample_record);
        }
    }

}

} // namespace arcxel