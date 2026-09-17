#include "plane.h"
#include "physics.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

namespace arcxel {

Plane::Plane(f32 length, f32 width, Transform transform) noexcept
    : GameObject(transform)
    , length(length * transform.scale.x)
    , width(width * transform.scale.z)
    , colour(LIGHTGRAY) {
    mesh = GenMeshPlane(width, length, 1, 1);
    model = LoadModelFromMesh(mesh);

    auto physics_pos = as(transform.translation);
    auto physics_transform = rp3d::Transform{ physics_pos, rp3d::Quaternion::identity() };

    body = Physics::singleton().world->createRigidBody(physics_transform);
    body->setType(rp3d::BodyType::STATIC);
    body->enableGravity(false);

    shape = Physics::singleton().common.createBoxShape(
        rp3d::Vector3(length * 0.5f, 0.00001f, width * 0.5f));

    collider = body->addCollider(shape, rp3d::Transform::identity());
    auto physics_mat = collider->getMaterial();
    physics_mat.setMassDensity(1.0f);
    physics_mat.setBounciness(0.0f);
    physics_mat.setFrictionCoefficient(0.8f);
    body->updateMassPropertiesFromColliders();
}


auto Plane::handle_events() -> void {};


auto Plane::update(f64) -> void {}


auto Plane::render(f64) -> void {
    auto axis = Vector3{};
    auto angle = f32{};
    QuaternionToAxisAngle(QuaternionNormalize(transform.rotation), &axis, &angle);

    rlDisableBackfaceCulling(); //<! Draw both sides of the plane

    DrawModelEx(
        model,
        transform.translation,
        axis,
        angle * RAD2DEG,
        transform.scale,
        colour);

    rlEnableBackfaceCulling();
}
} // namespace arcxel
