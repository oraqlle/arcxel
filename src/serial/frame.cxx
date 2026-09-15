// <frame.cxx> -*- C++ -*-

#include "serial/frame.h"
#include "engine.h"
#include "physics.h"
#include "timing.h"
#include "types.h"

namespace arcxel::serial {

    using Label = Sample::Label;

    auto run_frame(Engine& engine, f64 delta) -> void {
        // everything on main thread
        {
            const auto _ = Timespan(Label::Events, engine.sample_record);
            engine.handle_events();
        }

        {
            const auto _ = Timespan(Label::PhysicsUpdate, engine.sample_record);
            Physics::singleton().update(delta);
        }

        {
            const auto _ = Timespan(Label::Update, engine.sample_record);
            engine.update(delta);
        }

        {
            const auto _ = Timespan(Label::Render, engine.sample_record);
            engine.render(delta);
        }
        
    }

} // namespace arcxel::serial
