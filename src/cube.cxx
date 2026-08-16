#include <cube.h>

#include <raylib.h>

namespace arcxel {

Cube::Cube() noexcept
    : GameObject()
    , width(1.0f)
    , height(1.0f)
    , length(1.0f)
    , colour(RED) {
    mesh = GenMeshCube(width, height, length);
    model = LoadModelFromMesh(mesh);
};

Cube::Cube(Transform3D transform) noexcept
    : GameObject(transform)
    , width(1.0f)
    , height(1.0f)
    , length(1.0f)
    , colour(RED) {
    this->transform = transform;
    mesh = GenMeshCube(width, height, length);
    model = LoadModelFromMesh(mesh);
}

auto Cube::update(f64 delta) -> void {}

auto Cube::render(f64 delta) -> void {
    DrawModel(model, transform.position, 1.0f, colour);
}

} // namespace arcxel
