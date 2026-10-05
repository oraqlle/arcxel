#include "game_loop.h"
#include "conf.h"
#include "engine.h"
#include "physics.h"
#include "timing.h"
#include "log.h"

#include <raylib.h>


namespace arcxel {

using Label = Sample::Label;

auto game_loop([[maybe_unused]] Config config) -> void {

   log(LogLevel::Info, "Starting [BROAD] game loop");
    
    while (Engine::singleton().is_running()) {

        const auto frame_span = arcxel::Timespan(Label::Frame, Engine::singleton().sample_record);
        const f64 delta = GetFrameTime();

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
            Engine::singleton().update(delta);
        }

        {
            const auto _ = Timespan(Label::Render, Engine::singleton().sample_record);
            Engine::singleton().render(delta);
        }
    }
}

} // namespace arcxel