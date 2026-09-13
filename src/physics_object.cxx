#include "physics_object.h"
#include "physics.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

PhysicsObject::PhysicsObject(
    rp3d::BodyType btype,
    Transform transform,
    Color colour) noexcept
    : GameObject(transform)
    , colour(colour)
    , mesh({})
    , model({})
    , body(nullptr) {
    const auto phys_pos = as(transform.translation);
    auto phys_transform = rp3d::Transform{ phys_pos, rp3d::Quaternion::identity() };
    body = Physics::singleton().world->createRigidBody(phys_transform);
    body->setType(btype);
}

PhysicsObject::~PhysicsObject() noexcept {
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


auto PhysicsObject::handle_events() -> void {};


auto PhysicsObject::update(f64) -> void { _M_sync_model_to_physics(); }


auto PhysicsObject::render(f64) -> void {
    auto axis = Vector3{};
    auto angle = f32{};
    QuaternionToAxisAngle(QuaternionNormalize(transform.rotation), &axis, &angle);
    DrawModelEx(
        model,
        transform.translation,
        axis,
        angle * RAD2DEG,
        transform.scale,
        colour);
}


auto PhysicsObject::_M_sync_model_to_physics() -> void {
    transform = as(body->getTransform());
}

} // namespace arcxel
