#include "game_loop.h"
#include "conf.h"
#include "engine.h"
#include "physics.h"
#include "timing.h"
#include "log.h"

#include <raylib.h>


namespace arcxel {

using Label = Sample::Label;


static constexpr usize num_threads_req = 3;
static constexpr usize threaded_max_num_samples = SampleRecord::MAX_SAMPLES / 4;


static auto update_sim_worker() -> void {

    while (Engine::singleton().is_running()) {
    }
}


static auto physics_update_sim_worker() -> void {

    while (Engine::singleton().is_running()) {
    }
}


static auto serial_game_loop_fallback([[maybe_unused]] Config config, SampleRecord& global_sample_record) -> void {
	log(LogLevel::Info, "Starting [SERIAL:FALLBACK] game loop");
    
    while (Engine::singleton().is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, global_sample_record);
        const f64 delta = GetFrameTime();

        {
            const auto _ = Timespan(Label::PhysicsUpdate, global_sample_record);
            Physics::singleton().update(delta);
        }

        {
            const auto _ = Timespan(Label::Update, global_sample_record);
            Engine::singleton().update(delta);
        }

        {
            const auto _ = Timespan(Label::Render, global_sample_record);
            Engine::singleton().render(delta, global_sample_record);
        }
    }
}


auto game_loop([[maybe_unused]] Config config, SampleRecord& global_sample_record) -> void {

    log(LogLevel::Info, "Starting [BROAD] game loop");

	const auto num_threads_avail = std::thread::hardware_concurrency();
    if (num_threads_avail < num_threads_req) {
		log(LogLevel::Warning, "Not enough threads to run [BROAD] game loop");
        return serial_game_loop_fallback(config, global_sample_record);
    }

    auto frame_and_render_samples = SampleRecord(threaded_max_num_samples * 2);
    auto physics_samples = SampleRecord(threaded_max_num_samples);
    auto update_samples = SampleRecord(threaded_max_num_samples);


    // ---- Physics Thread ----


    // ---- Update Thread ----


    // ---- Main Thread: Render ----
    while (Engine::singleton().is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, global_sample_record);
        const f64 delta = GetFrameTime();

        {
            const auto _ = Timespan(Label::PhysicsUpdate, physics_samples);
            Physics::singleton().update(delta);
        }

        {
            const auto _ = Timespan(Label::Update, update_samples);
            Engine::singleton().update(delta);
        }

        {
            const auto _ = Timespan(Label::Render, global_sample_record);
            Engine::singleton().render(delta, global_sample_record);
        }
    }

    global_sample_record += frame_and_render_samples;
    global_sample_record += physics_samples;
    global_sample_record += update_samples;
}


} // namespace arcxel