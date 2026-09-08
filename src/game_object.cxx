#include "game_object.h"

#include <raylib.h>

namespace arcxel {

GameObject::GameObject() noexcept
    : transform(
          {.translation = Vector3{0.0f, 0.0f, 0.0f},
           .rotation = Quaternion{0.0f, 0.0f, 0.0f, 0.0f},
           .scale = Vector3{1.0f, 1.0f, 1.0f}}
      ) {}

GameObject::GameObject(Transform transform)
    : transform(transform) {}

} // namespace arcxel
