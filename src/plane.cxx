#include "plane.h"
#include "physics.h"
#include "physics_object.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

namespace arcxel {

Plane::Plane(f32 length, f32 width, Transform transform) noexcept
    : PhysicsObject(rp3d::BodyType::STATIC, transform, LIGHTGRAY)
    , length(length * transform.scale.x)
    , width(width * transform.scale.z) {
    body->enableGravity(false);

    _M_create_mesh();
    _M_create_collision_shape();
}


Plane::~Plane() noexcept {
    if (body && collider) {
        body->removeCollider(collider);
        collider = nullptr;
    }

    if (shape) {
        Physics::singleton().common.destroyBoxShape(shape);
        shape = nullptr;
    }
}


auto Plane::_M_create_mesh() -> void {
    mesh = GenMeshPlane(length, width, 1, 1);
    model = LoadModelFromMesh(mesh);

    mesh = {};
}


auto Plane::_M_create_collision_shape() -> void {
    shape = Physics::singleton().common.createBoxShape(
        rp3d::Vector3(length * 0.5f, 0.00001f, width * 0.5f));

    collider = body->addCollider(shape, rp3d::Transform::identity());

    auto physics_mat = collider->getMaterial();
    physics_mat.setMassDensity(1.0f);
    physics_mat.setBounciness(0.0f);
    physics_mat.setFrictionCoefficient(0.8f);
    body->updateMassPropertiesFromColliders();
}


auto Plane::render(f64 delta) -> void {
    rlDisableBackfaceCulling(); //<! Draw both sides of the plane
    PhysicsObject::render(delta);
    rlEnableBackfaceCulling();
}
} // namespace arcxel
