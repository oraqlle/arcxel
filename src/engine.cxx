#include "engine.h"

#include "timing.h"

#include <raylib.h>

namespace arcxel {

Engine::Engine()
    : running(true) {}

[[nodiscard]] auto Engine::is_running() -> bool {
    return running && !WindowShouldClose();
}

auto Engine::stop() -> void { running = false; }

auto Engine::handle_events() -> void { scene.handle_events(); }

auto Engine::update(f64 delta) -> void { scene.update(delta); }

auto Engine::render(f64 delta, SampleRecord& store) -> void {
    auto camera = scene.primary_camera();

    {
        const auto span = Timespan(Sample::Label::Construct, store);
        BeginDrawing();
        ClearBackground(RAYWHITE);
    }

    {
        const auto span = Timespan(Sample::Label::Draw, store);
        BeginMode3D(camera);
        scene.render(delta);
        EndMode3D();
    }

    {
        const auto span = Timespan(Sample::Label::Present, store);
        EndDrawing();
    }
}

} // namespace arcxel
