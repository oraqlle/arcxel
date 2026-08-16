#include "game_object.h"

#include <raylib.h>

namespace arcxel {

GameObject::GameObject(Transform3D transform)
    : transform(transform) {}

auto GameObject::handle_events() -> void {}

auto GameObject::update(f64 delta) -> void {}

auto GameObject::render(f64 delta) -> void {}

} // namespace arcxel
