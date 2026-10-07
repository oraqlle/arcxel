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
        log(LogLevel::Info, "Starting [SERIAL] game loop [FALLBACK OF BROAD]");
        return serial_game_loop_fallback(config, global_sample_record);
    }

    log(LogLevel::Info, "Starting [BROAD] game loop");

	//auto& pool = ThreadPool::singleton();
	auto& scene = Engine::singleton().get_scene();

    auto simulation_queue = Queue<usize>{};
    auto render_queue = Queue<usize>{};

    while (Engine::singleton().is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, global_sample_record);
        const f64 delta = GetFrameTime();


		{
			const auto _ = Timespan(Label::PhysicsUpdate, global_sample_record);
			Physics::singleton().update(delta);
		}


        {
            const auto _ = Timespan(Label::Update, global_sample_record);
            const unsigned workerCount = std::thread::hardware_concurrency();
            std::vector<std::thread> workers;

            for (unsigned i = 0; i < workerCount; ++i) {
                workers.emplace_back([&] {
                    while (auto idx = simulation_queue.pop()) {
                        if (!idx.has_value()) {
                            break;
                        }

                        //auto* p = dynamic_cast<PhysicsObject*>(scene.objects[*idx].get());
						//if (p) { p->tint = RED; }
                        scene.objects[*idx]->update(delta);
                    }
                });
            }

            for (auto idx : std::views::iota(usize{ 0 }, scene.objects.size())) {
                simulation_queue.push(idx);
            }

            simulation_queue.close();

            for (auto& worker : workers) {
                worker.join();
            }

            workers.clear();
        }
		

		{
            const auto _ = Timespan(Label::Render, global_sample_record);
            Engine::singleton().render(delta, global_sample_record);
        }
    }

		//log(LogLevel::Info, "[BROAD] update");
        //{
        //    const auto _ = Timespan(Label::Update, global_sample_record);

        //    for (auto idx : std::views::iota(usize{ 0 }, scene.objects.size())) {
        //        simulation_queue.push(idx);
        //    }

		//	log(LogLevel::Info, "[BROAD] update filled");
		//	while (auto idx = simulation_queue.pop()) { // never breaks loop?
		//		if (!idx.has_value()) {
		//			break;
		//		}

		//		pool.submit([&idx, &scene, delta] {
		//			scene.objects[*idx]->update(delta);
		//			log(LogLevel::Info, "[BROAD] idx: {}", *idx);
		//		});
		//	}

		//	log(LogLevel::Info, "[BROAD] update wait");
        //    pool.wait();
        //}

        //{
		//	const auto _ = Timespan(Label::Render, global_sample_record);
		//	auto& camera = scene.primary_camera();

		//	{
		//		const auto span = Timespan(Sample::Label::Construct, global_sample_record);
		//		BeginDrawing();
		//		ClearBackground(RAYWHITE);
		//	}

		//	{
		//		const auto span = Timespan(Sample::Label::Draw, global_sample_record);
		//		BeginMode3D(camera);

		//		DrawGrid(200, 1.0f);

		//		auto x_axis = Vector3{ 1000.0f, 0.0f, 0.0f };
		//		auto y_axis = Vector3{ 0.0f, 1000.0f, 0.0f };
		//		auto z_axis = Vector3{ 0.0f, 0.0f, 1000.0f };

		//		DrawLine3D(x_axis, Vector3Zeros - x_axis, RED);
		//		DrawLine3D(y_axis, Vector3Zeros - y_axis, GREEN);
		//		DrawLine3D(z_axis, Vector3Zeros - z_axis, BLUE);

		//		while (auto opt = render_queue.pop()) {
		//			if (!opt) {
		//				break;
		//			}

		//			auto idx = *opt;
		//			pool.submit([&scene, idx, delta] {
		//				scene.objects[idx]->render(delta);
		//			});
		//		}

		//		pool.wait();

		//		EndMode3D();
		//	}

		//	{
		//		const auto span = Timespan(Sample::Label::Present, global_sample_record);
		//		EndDrawing();
		//	}
		//}
}

} // namespace arcxel