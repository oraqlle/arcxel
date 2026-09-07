#include "scene.h"
#include "cube.h"
#include "game_object.h"
#include "player.h"
#include "transform.h"
#include "types.h"

#include <random>
#include <ranges>
#include <raylib.h>

namespace arcxel {

Scene::Scene() noexcept {
    auto player = Player();

    _M_generate_objects(1);
}


Scene::Scene(usize num_objects) noexcept {
    auto player = Player();

    _M_generate_objects(num_objects);
}


auto Scene::handle_events() -> void {
    for (auto& obj : objects) {
        obj->handle_events();
    }

    player.handle_events();
}


auto Scene::update(f64 delta) -> void {
    for (auto& obj : objects) {
        obj->update(delta);
    }

    player.update(delta);
}


auto Scene::render(f64 delta) -> void {
    DrawGrid(10000, 1.0f);


    for (auto& obj : objects) {
        obj->render(delta);
    }

    player.render(delta);
}


[[nodiscard]] auto Scene::primary_camera() -> Camera3D { return player.get_camera(); }


auto Scene::_M_generate_objects(usize num_objects) -> void {
    auto rand = std::default_random_engine(std::random_device{}());
    auto xdist = std::uniform_real_distribution<float>(-100.0f, 100.0f);
    auto ydist = std::uniform_real_distribution<float>(-100.0f, 100.0f);
    auto zdist = std::uniform_real_distribution<float>(-50.0f, 50.0f);

    for (auto _ : std::views::iota(num_objects) | std::views::take(num_objects)) {
        auto x = xdist(rand);
        auto y = ydist(rand);
        auto z = zdist(rand);
        auto translation = Vector3{.x = x, .y = y, .z = z};
        auto scale = Vector3{.x = 5, .y = 5, .z = 5};
        //auto translation = Vector3{.x = 0.0f, .y = 0.0f, .z = 0.0f};

        auto transform = Transform3D();
        transform.translate(translation);
        transform.scale(scale);

        auto cube = std::make_unique<Cube>(transform);
        objects.push_back(std::move(cube));
    }
}

} // namespace arcxel
