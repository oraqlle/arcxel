#include "sphere.h"
#include "physics.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

Sphere::Sphere(Transform transform, Color colour) noexcept
    : PhysicsObject(rp3d::BodyType::DYNAMIC, transform, colour)
    , radius(transform.scale.x) { //< only use scale::x field to scale radius
    _M_create_mesh();
    _M_create_collision_shape();
}


Sphere::~Sphere() noexcept {
    if (body && collider) {
        body->removeCollider(collider);
        collider = nullptr;
    }

    if (shape) {
        Physics::singleton().common.destroySphereShape(shape);
        shape = nullptr;
    }
}


auto Sphere::_M_create_mesh() -> void {
    mesh = GenMeshSphere(radius, 8, 12);
    model = LoadModelFromMesh(mesh);

    mesh = {};
}


auto Sphere::_M_create_collision_shape() -> void {
    shape = Physics::singleton().common.createSphereShape(radius * 0.5f);
    collider = body->addCollider(shape, rp3d::Transform::identity());
    collider->getMaterial().setMassDensity(1.0f);
    body->updateMassPropertiesFromColliders();
}

} // namespace arcxel
