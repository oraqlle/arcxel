#include <log/logger.h>
#include <timing/registry.h>
#include <timing/span.h>
#include <triangle.h>
#include <window.h>

#include "raylib.h"

#include <exception>

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

namespace {

auto run() -> void {
    auto window = arcxel::Window(WIDTH, HEIGHT, "Arcxel Window");
    arcxel::log::info("window opened {}x{}", WIDTH, HEIGHT);
    auto centre = Vector2{WIDTH / 2.0f, HEIGHT / 2.0f};

    auto v1 = Vector2{centre.x, centre.y - 175};
    auto v2 = Vector2{centre.x - 200, centre.y + 175};
    auto v3 = Vector2{centre.x + 200, centre.y + 175};
    auto tri = arcxel::Triangle(v1, v2, v3, ORANGE);

    // ~4M samples, about ten minutes of uncapped frames. Samples past this are
    // dropped, not reallocated.
    arcxel::timing::reserve(1U << 22U);
    const auto frame_span = arcxel::timing::register_label("frame");
    const auto begin_span = arcxel::timing::register_label("begin");
    const auto clear_span = arcxel::timing::register_label("clear");
    const auto draw_span = arcxel::timing::register_label("draw");
    const auto present_span = arcxel::timing::register_label("present");

    while (!WindowShouldClose()) {
        ARCXEL_SPAN(frame_span);

        {
            ARCXEL_SPAN(begin_span);
            BeginDrawing();
        }

        {
            ARCXEL_SPAN(clear_span);
            ClearBackground(RAYWHITE);
        }

        {
            ARCXEL_SPAN(draw_span);
            tri.draw();
        }

        {
            ARCXEL_SPAN(present_span);
            EndDrawing();
        }
    }

    arcxel::timing::log_summary();
    arcxel::timing::write_run_csv();
}

} // namespace

auto main() -> int {
    arcxel::log::adopt_raylib();

    try {
        run();
    } catch (const std::exception& error) {
        arcxel::log::fatal("{}", error.what());
        return 1;
    } catch (...) {
        arcxel::log::fatal("terminated by an unknown exception");
        return 1;
    }

    return 0;
}
