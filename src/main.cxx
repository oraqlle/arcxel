#include <log/logger.h>
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

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        tri.draw();
        EndDrawing();
    }

    return 0;
}
