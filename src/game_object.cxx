#include "game_object.h"

#include <raylib.h>

namespace arcxel {

GameObject::GameObject(Transform transform) noexcept
    : transform(transform) {}

} // namespace arcxel
