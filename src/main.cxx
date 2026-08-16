#include "engine.h"
#include "types.h"
#include "window_info.h"

#include <raylib.h>

// clang-format off
using arcxel::i8;
using arcxel::i16;
using arcxel::i32;
using arcxel::i64;

using arcxel::u8;
using arcxel::u16;
using arcxel::u32;
using arcxel::u64;

using arcxel::isize;
using arcxel::usize;

using arcxel::f32;
using arcxel::f64;
// clang-format on

constexpr i32 WIDTH = 1920;
constexpr i32 HEIGHT = 1080;

auto main() -> int {
    auto winfo = arcxel::WindowInfo{.width = WIDTH, .height = HEIGHT};
    InitWindow(winfo.width, winfo.height, winfo.name.c_str());
    SetTargetFPS(winfo.target_fps);
    DisableCursor();

    auto engine = arcxel::Engine();

    while (engine.is_running()) {
        engine.handle_events();

        // obtain delta
        f64 delta = GetFrameTime();

        engine.update(delta);

        engine.render(delta);
    }

    EnableCursor();
    CloseWindow();

    return 0;
}
