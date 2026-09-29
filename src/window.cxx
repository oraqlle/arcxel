#include "window.h"

#include "window_info.h"

#include <raylib.h>

#include <expected>
#include <string>
#include <utility>

namespace arcxel {

Window::Window(Token) noexcept {}

Window::~Window() { CloseWindow(); }

auto Window::create(const WindowInfo& info) -> std::expected<Window, std::string> {
    InitWindow(info.width, info.height, info.name.c_str());

    if (!IsWindowReady()) {
        // InitWindow may have got part way, so close before reporting.
        CloseWindow();
        return std::unexpected("arcxel: window failed to initialise");
    }

    SetTargetFPS(info.target_fps); // 0 leaves the frame rate uncapped

    return std::expected<Window, std::string>(std::in_place, Token{});
}

} // namespace arcxel
