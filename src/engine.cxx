#include "engine.h"

#include <raylib.h>

namespace arcxel {

Engine::Engine() noexcept
    : running(true) {};

[[nodiscard]] auto Engine::is_running() -> bool {
    return running && !WindowShouldClose();
}

auto Engine::stop() -> void { running = false; }

auto Engine::handle_events() -> void { scene.handle_events(); }

auto Engine::update(f64 delta) -> void { scene.update(delta); }

auto Engine::render(f64 delta) -> void {
    auto camera = scene.primary_camera();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    scene.render(delta);

    EndMode3D();

    EndDrawing();
}

} // namespace arcxel
