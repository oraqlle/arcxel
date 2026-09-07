#include "cube.h"
#include "physics.h"

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

    const auto physics_pos = rp3d::Vector3(
        transform.position.x,
        transform.position.y,
        transform.position.z
    );

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


Cube::Cube(Transform3D transform) noexcept
    : GameObject(transform)
    , width(1.0f * transform.scaler.x)
    , height(1.0f * transform.scaler.y)
    , length(1.0f * transform.scaler.z)
    , colour(RED) {
    this->transform = transform;
    mesh = GenMeshCube(width, height, length);
    model = LoadModelFromMesh(mesh);

    const auto physics_pos = rp3d::Vector3(
        transform.position.x,
        transform.position.y,
        transform.position.z
    );

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
    const auto& physics_transform = body->getTransform();

    transform.position = Vector3{
        .x = physics_transform.getPosition().x,
        .y = physics_transform.getPosition().y,
        .z = physics_transform.getPosition().z
    };

    transform.rotation = Quaternion{
        .x = physics_transform.getOrientation().x,
        .y = physics_transform.getOrientation().y,
        .z = physics_transform.getOrientation().z,
        .w = physics_transform.getOrientation().w
    };
}


auto Cube::render(f64 delta) -> void {
    auto axis = Vector3{};
    auto angle = f32{};
    QuaternionToAxisAngle(transform.rotation, &axis, &angle);

    DrawModelEx(model, transform.position, axis, angle, transform.scaler, colour);
}


auto Cube::set_gravity(bool on) -> void {
    body->enableGravity(on);
}


auto Cube::set_body_type(rp3d::BodyType type) -> void {
    body->setType(type);
}

} // namespace arcxel
