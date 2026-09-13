#include "floor.h"
#include "physics.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

Floor::Floor(f32 width, f32 length) noexcept
    : GameObject(TransformIdentity)
    , width(width * transform.scale.x)
    , length(length * transform.scale.z)
    , colour(LIGHTGRAY) {
    mesh = GenMeshPlane(width, length, 1, 1);
    model = LoadModelFromMesh(mesh);

    auto physics_pos = as(transform.translation);
    auto physics_transform = rp3d::Transform{ physics_pos, rp3d::Quaternion::identity() };

    body = Physics::singleton().world->createRigidBody(physics_transform);
    body->setType(rp3d::BodyType::STATIC);
    body->enableGravity(false);

    shape = Physics::singleton().common.createBoxShape(
        rp3d::Vector3(width * 0.5f, 0.00001f, length * 0.5f));

    collider = body->addCollider(shape, rp3d::Transform::identity());
    auto physics_mat = collider->getMaterial();
    physics_mat.setMassDensity(1.0f);
    physics_mat.setBounciness(0.0f);
    physics_mat.setFrictionCoefficient(0.8f);
    body->updateMassPropertiesFromColliders();
}


auto Floor::handle_events() -> void {};


auto Floor::update(f64) -> void {}


auto Floor::render(f64) -> void {
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
} // namespace arcxel
