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

auto game_loop(Config config) -> void {

	log(LogLevel::Info, "Starting [FINE] game loop");
    
    while (Engine::singleton().is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, Engine::singleton().sample_record);
        const f64 delta = GetFrameTime();

		auto& pool = ThreadPool::singleton();
		auto& scene = Engine::singleton().get_scene();

		{
			const auto _ = Timespan(Label::Events, Engine::singleton().sample_record);
			Engine::singleton().handle_events();
		}

		{
			const auto _ = Timespan(Label::PhysicsUpdate, Engine::singleton().sample_record);
			Physics::singleton().update(delta);
		}

		{
			const auto _ = Timespan(Label::Update, Engine::singleton().sample_record);

			const auto count = scene.objects.size() - 1; //< minus player
			const auto nchunks = pool.size();
			const auto per_chunk = (count + nchunks - 1) / nchunks;

			for (auto chunk : std::views::iota(usize{ 0 }, nchunks)) {
				const auto first = (chunk * per_chunk) + 1;
				const auto last = std::min(first + per_chunk, scene.objects.size()); //< Use true count to get final object

				if (first >= last) {
					continue;
				}

				pool.submit(
					[&scene, first, last, delta] { scene.update_range(first, last, delta); });
			}

			pool.wait();
			scene.update_range(0, 1, delta); //< update player
		}

		{
			const auto _ = Timespan(Label::Render, Engine::singleton().sample_record);
			Engine::singleton().render(delta);
		}
    }
}

} // namespace arcxel
