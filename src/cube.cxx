#include "cube.h"
#include "physics.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

Cube::Cube(Transform transform, Color colour) noexcept
    : Shape(rp3d::BodyType::DYNAMIC, transform, colour)
    , size(Vector3Ones * transform.scale) {
    _M_create_mesh();
    _M_create_collision_shape();
}


Cube::~Cube() noexcept {
    if (body && collider) {
        body->removeCollider(collider);
        collider = nullptr;
    }

    if (shape) {
        Physics::singleton().common.destroyBoxShape(dynamic_cast<shape_t*>(shape));
        shape = nullptr;
    }
}


auto Cube::_M_create_mesh() -> void {
    mesh = GenMeshCube(size.x, size.y, size.z);
    model = LoadModelFromMesh(mesh);

    mesh = {};
}


auto Cube::_M_create_collision_shape() -> void {
    shape = Physics::singleton().common.createBoxShape(as(size * 0.5));
    collider = body->addCollider(shape, rp3d::Transform::identity());
    collider->getMaterial().setMassDensity(1.0f);
    body->updateMassPropertiesFromColliders();
}

} // namespace arcxel
