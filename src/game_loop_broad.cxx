#include "game_loop.h"
#include "conf.h"
#include "engine.h"
#include "physics.h"
#include "timing.h"
#include "log.h"

#include <raylib.h>


namespace arcxel {

using Label = Sample::Label;


static auto update_sim_worker() -> void {

    while (Engine::singleton().is_running()) {
    }
}


static auto physics_update_sim_worker() -> void {

    while (Engine::singleton().is_running()) {
    }
}


auto game_loop([[maybe_unused]] Config config, SampleRecord& global_sample_record) -> void {

    log(LogLevel::Info, "Starting [BROAD] game loop");
    
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

} // namespace arcxel