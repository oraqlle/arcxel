#include <scene.h>

#include <raylib.h>

namespace arcxel {

Scene::Scene() noexcept { cube.transform.position.z = -15.0f; }

auto Scene::handle_events() -> void {
    cube.handle_events();
    player.handle_events();
}

auto Scene::update(f64 delta) -> void {
    cube.update(delta);
    player.update(delta);
}

auto Scene::render(f64 delta) -> void {
    DrawGrid(10000, 1.0f);
    cube.render(delta);
    player.render(delta);
}

[[nodiscard]] auto Scene::primary_camera() -> Camera3D { return player.get_camera(); }

} // namespace arcxel
