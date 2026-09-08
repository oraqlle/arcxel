#include "cube.h"
#include "physics.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

Cube::Cube() noexcept
    : GameObject()
    , width(1.0f)
    , height(1.0f)
    , length(1.0f)
    , colour(RED) {
    mesh = GenMeshCube(width, height, length);
    model = LoadModelFromMesh(mesh);

    const auto physics_pos = as(transform.translation);
    auto physics_transform = rp3d::Transform{physics_pos, rp3d::Quaternion::identity()};

    body = Physics::singleton().world->createRigidBody(physics_transform);
    body->setType(rp3d::BodyType::DYNAMIC);

    shape = Physics::singleton().common.createBoxShape(rp3d::Vector3(
        width * 0.5f,
        height * 0.5f,
        length * 0.5f
    ));

    collider = body->addCollider(shape, rp3d::Transform::identity());
    collider->getMaterial().setMassDensity(1.0f);
    body->updateMassPropertiesFromColliders();
};


Cube::Cube(Transform transform) noexcept
    : GameObject(transform)
    , width(1.0f * transform.scale.x)
    , height(1.0f * transform.scale.y)
    , length(1.0f * transform.scale.z)
    , colour(RED) {
    mesh = GenMeshCube(width, height, length);
    model = LoadModelFromMesh(mesh);

    const auto physics_pos = as(transform.translation);
    auto physics_transform = rp3d::Transform{physics_pos, rp3d::Quaternion::identity()};

    body = Physics::singleton().world->createRigidBody(physics_transform);
    body->setType(rp3d::BodyType::DYNAMIC);

    shape = Physics::singleton().common.createBoxShape(rp3d::Vector3(
        width * 0.5f,
        height * 0.5f,
        length * 0.5f
    ));

    collider = body->addCollider(shape, rp3d::Transform::identity());
    collider->getMaterial().setMassDensity(1.0f);
    body->updateMassPropertiesFromColliders();
}


auto Cube::handle_events() -> void {};


auto Cube::update(f64 delta) -> void {
    transform = as(body->getTransform());
}


auto Cube::render(f64 delta) -> void {
    auto axis = Vector3{};
    auto angle = f32{};
    QuaternionToAxisAngle(transform.rotation, &axis, &angle);
    DrawModelEx(model, transform.translation, axis, angle, transform.scale, colour);
}

} // namespace arcxel
