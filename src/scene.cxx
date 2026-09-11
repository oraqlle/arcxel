#include "scene.h"
#include "cube.h"
#include "floor.h"
#include "game_object.h"
#include "player.h"
#include "types.h"

#include <raylib.h>

#include <random>
#include <ranges>

namespace arcxel {

Scene::Scene() noexcept {
    auto player = Player();

    _M_create_floor();
    _M_generate_objects(1);
}


Scene::Scene(usize num_objects) noexcept {
    auto player = Player();

    _M_create_floor();
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
    DrawGrid(200, 1.0f);


    for (auto& obj : objects) {
        obj->render(delta);
    }

    player.render(delta);
}


[[nodiscard]] auto Scene::primary_camera() -> Camera3D { return player.get_camera(); }


auto Scene::unload() -> void { objects.clear(); }


auto Scene::_M_create_floor() -> void {
    auto floor = std::make_unique<Floor>();
    objects.push_back(std::move(floor));
}


auto Scene::_M_generate_objects(usize num_objects) -> void {
    auto rand = std::default_random_engine(std::random_device{}());
    auto xdist = std::uniform_real_distribution<float>(-50.0f, 50.0f);
    auto ydist = std::uniform_real_distribution<float>(10.0f, 50.0f);
    auto zdist = std::uniform_real_distribution<float>(-50.0f, 50.0f);

    for (auto _ : std::views::iota(num_objects) | std::views::take(num_objects)) {
        auto translation = Vector3{.x = xdist(rand), .y = ydist(rand), .z = zdist(rand)};

        auto transform = Transform{
            .translation = translation,
            .rotation = QuaternionUnitX,
            .scale = Vector3{1.0f, 1.0f, 1.0f}
        };

        auto colour = Color{
            .r = static_cast<unsigned char>(std::abs(translation.x / 50.0f) * 255.0f),
            .g = static_cast<unsigned char>(std::abs(translation.y / 50.0f) * 255.0f),
            .b = static_cast<unsigned char>(std::abs(translation.z / 50.0f) * 255.0f),
            .a = 255
        };

        auto cube = std::make_unique<Cube>(transform, colour);
        objects.push_back(std::move(cube));
    }
}

} // namespace arcxel
