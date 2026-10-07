#include "game_loop.h"
#include "conf.h"
#include "engine.h"
#include "physics.h"
#include "timing.h"
#include "thread_pool.h"
#include "log.h"

#include <raylib.h>


namespace arcxel {

using Label = Sample::Label;

auto game_loop([[maybe_unused]] Config config, SampleRecord& global_sample_record) -> void {

	log(LogLevel::Info, "Starting [STATIC_PARTITIONING] game loop");
    
    while (Engine::singleton().is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, global_sample_record);
        const f64 delta = GetFrameTime();

		auto& pool = ThreadPool::singleton();
		auto& scene = Engine::singleton().get_scene();

		{
			const auto _ = Timespan(Label::PhysicsUpdate, global_sample_record);
			Physics::singleton().update(delta);
		}

		{
			const auto _ = Timespan(Label::Update, global_sample_record);

			const auto count = scene.objects.size() - 1; //< skip last object (player)
			const auto nchunks = pool.size();
			const auto per_chunk = (count + nchunks - 1) / nchunks;

			for (auto chunk : std::views::iota(usize{ 0 }, nchunks)) {
				const auto first = (chunk * per_chunk);
				const auto last = std::min(first + per_chunk, count);

				if (first >= last) {
					continue;
				}

				pool.submit(
					[&scene, first, last, delta] { scene.update_range(first, last, delta); });
			}

			pool.wait();
			scene.update_range(count, scene.objects.size(), delta); //< update last object (player)
		}

		{
			const auto _ = Timespan(Label::Render, global_sample_record);
			Engine::singleton().render(delta, global_sample_record);
		}
    }
}

} // namespace arcxel
