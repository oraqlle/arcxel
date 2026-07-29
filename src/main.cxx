#include <window.h>

#include "raylib.h"

#include <print>

auto main() -> int {
    std::println("Hello, World!");

    auto window = arcxel::Window(800, 600, "Arcxel Window");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    return 0;
}
