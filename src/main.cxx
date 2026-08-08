#include <log/logger.h>
#include <timing/registry.h>
#include <timing/span.h>
#include <triangle.h>
#include <window.h>

#include "raylib.h"

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

auto main() -> int {
    arcxel::log::adopt_raylib();

    auto window = arcxel::Window(WIDTH, HEIGHT, "Arcxel Window");
    arcxel::log::info("window opened {}x{}", WIDTH, HEIGHT);
    auto centre = Vector2{WIDTH / 2.0f, HEIGHT / 2.0f};

    auto v1 = Vector2{centre.x, centre.y - 175};
    auto v2 = Vector2{centre.x - 200, centre.y + 175};
    auto v3 = Vector2{centre.x + 200, centre.y + 175};
    auto tri = arcxel::Triangle(v1, v2, v3, ORANGE);

    arcxel::timing::reserve(1U << 16U);
    const auto frame_span = arcxel::timing::register_label("frame");
    const auto draw_span = arcxel::timing::register_label("draw");

    while (!WindowShouldClose()) {
        ARCXEL_SPAN(frame_span);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        {
            ARCXEL_SPAN(draw_span);
            tri.draw();
        }

        EndDrawing();
    }

    arcxel::timing::log_summary();
    arcxel::timing::write_csv("arcxel-timing.csv");

    return 0;
}
