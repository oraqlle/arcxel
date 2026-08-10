#include <engine.h>
#include <quad.h>
#include <triangle2d.h>
#include <window.h>

#include "raylib.h"

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

auto main() -> int {
    auto window = arcxel::Window(WIDTH, HEIGHT, "Arcxel Window");
    auto engine = arcxel::Engine(std::move(window));

    while (engine.is_running()) {
        engine.handle_events();

        // obtain delta
        double delta = GetFrameTime();

        engine.update(delta);

        engine.render(delta);
    }

    return 0;
}
