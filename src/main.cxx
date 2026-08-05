#include <quad.h>
#include <triangle2d.h>
#include <window.h>

#include "raylib.h"

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

auto main() -> int {
    auto window = arcxel::Window(WIDTH, HEIGHT, "Arcxel Window");
    auto centre = Vector2{WIDTH / 2.0f, HEIGHT / 2.0f};

    auto v1 = Vector2(0.0f, 175.0f);
    auto v2 = Vector2(-200.0f, -175.0f);
    auto v3 = Vector2(200.0f, -175.0f);
    auto tri = arcxel::Triangle2D(v1, v2, v3, ORANGE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        tri.render();
        EndDrawing();
    }

    return 0;
}
