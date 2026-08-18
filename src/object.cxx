#include "object.h"

#include <raylib.h>

namespace arcxel {

Object::Object() noexcept
    : classtype{"", 0} {}

Object::Type Object::type() { return classtype; }

} // namespace arcxel
