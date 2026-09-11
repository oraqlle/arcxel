#include "shape.h"
#include "physics.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

Shape::Shape(rp3d::BodyType btype, Transform transform, Color colour) noexcept
    : GameObject(transform)
    , colour(colour)
    , mesh({})
    , model({})
    , body(Shape::_M_create_rigid_body(btype, transform))
    , shape(nullptr)
    , collider(nullptr) {}


Shape::~Shape() noexcept {
    if (body) {
        Physics::singleton().world->destroyRigidBody(body);
        body = nullptr;
    }

    if (IsModelValid(model)) {
        UnloadModel(model);

        model = {};
        mesh = {};
    }
}


auto Shape::handle_events() -> void {};


auto Shape::update(f64) -> void { transform = as(body->getTransform()); }


auto Shape::render(f64) -> void {
    auto axis = Vector3{};
    auto angle = f32{};
    QuaternionToAxisAngle(transform.rotation, &axis, &angle);
    DrawModelEx(model, transform.translation, axis, angle, transform.scale, colour);
}

} // namespace arcxel
