#include <engine.h>
#include <types.h>
#include <window.h>

#include <raylib.h>

using arcxel::i8;
using arcxel::i16;
using arcxel::i32;
using arcxel::i64;

using arcxel::u8;
using arcxel::u16;
using arcxel::u32;
using arcxel::u64;

using arcxel::usize;
using arcxel::isize;

using arcxel::f32;
using arcxel::f64;

constexpr i32 WIDTH = 1920;
constexpr i32 HEIGHT = 1080;

auto main() -> int {
    auto window = arcxel::Window(WIDTH, HEIGHT, "Arcxel Window");
    auto engine = arcxel::Engine(std::move(window));

    while (engine.is_running()) {
        engine.handle_events();

        // obtain delta
        f64 delta = GetFrameTime();

        engine.update(delta);

        engine.render(delta);
    }

    return 0;
}
