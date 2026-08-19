#include "engine.h"

#include "timing.h"

#include <raylib.h>

namespace arcxel {

Engine::Engine()
    : running(true)
    , begin_label(timing::register_label("begin"))
    , clear_label(timing::register_label("clear"))
    , draw_label(timing::register_label("draw"))
    , present_label(timing::register_label("present")) {}

[[nodiscard]] auto Engine::is_running() -> bool {
    return running && !WindowShouldClose();
}

auto Engine::stop() -> void { running = false; }

auto Engine::handle_events() -> void { scene.handle_events(); }

auto Engine::update(f64 delta) -> void { scene.update(delta); }

auto Engine::render(f64 delta) -> void {
    auto camera = scene.primary_camera();

    {
        const auto span = timing::Span(begin_label);
        BeginDrawing();
    }

    {
        const auto span = timing::Span(clear_label);
        ClearBackground(RAYWHITE);
    }

    {
        const auto span = timing::Span(draw_label);
        BeginMode3D(camera);
        scene.render(delta);
        EndMode3D();
    }

    {
        const auto span = timing::Span(present_label);
        EndDrawing();
    }
}

} // namespace arcxel
