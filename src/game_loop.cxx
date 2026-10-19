#include "game_loop.h"
#include "conf.h"
#include "engine.h"
#include "physics.h"
#include "timing.h"
#include "log.h"

using Label = arcxel::Sample::Label;

namespace arcxel {

auto serial_game_loop_fallback(
    [[maybe_unused]] Config config,
    SampleRecord& global_sample_record) -> void {

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
