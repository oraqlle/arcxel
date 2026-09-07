#include "game_object.h"

#include <raylib.h>

namespace arcxel {

GameObject::GameObject(Transform3D transform)
    : transform(transform) {}

} // namespace arcxel
