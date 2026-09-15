// <frame.cxx> -*- C++ -*-

#include "broad/frame.h"
#include "broad_thread_pool.h"
#include "engine.h"
#include "physics.h"
#include "scene.h"
#include "timing.h"
#include "types.h"

#include <algorithm>
#include <ranges>

namespace arcxel::broad {

using Label = Sample::Label;

auto run_frame(Engine& engine, f64 delta) -> void {
    auto& pool = BroadThreadPool::singleton();
    auto& scene = engine.get_scene();

    // main thread only
    {
        const auto _ = Timespan(Label::Events, engine.sample_record);
        engine.handle_events();
    }

    // serial -> writes the transforms update reads next
    {
        const auto _ = Timespan(Label::PhysicsUpdate, engine.sample_record);
        Physics::singleton().update(delta);
    }

    // only parallel stage
    {
        const auto _ = Timespan(Label::Update, engine.sample_record);

        const auto count = scene.objects.size();
        const auto chunks = pool.size();
        const auto per_chunk = (count + chunks - 1) / chunks; // ceiling divide

        for (auto chunk : std::views::iota(usize{0}, chunks)) {
            const auto first = chunk * per_chunk;
            const auto last = std::min(first + per_chunk, count);

            // fewer objects than workers
            if (first >= last) {
                continue;
            }

            pool.submit([&scene, first, last, delta] {
                scene.update_range(first, last, delta);
            });
        }

        pool.wait();

        // main thread only
        scene.update_player(delta);
    }

    // main thread only
    {
        const auto _ = Timespan(Label::Render, engine.sample_record);
        engine.render(delta);
    }
}

} // namespace arcxel::broad