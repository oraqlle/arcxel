#include "scene.h"
#include "cube.h"
#include "game_object.h"
#include "player.h"
#include "transform.h"
#include "types.h"

#include <raylib.h>

#include <random>
#include <ranges>

namespace arcxel {

constexpr u32 SCENE_SEED = 20260913;

Scene::Scene() noexcept {
    auto player = Player();

    _M_create_floor();
    _M_generate_objects(1, Workload{});
}


Scene::Scene(usize num_objects, Workload workload) noexcept {
    auto player = Player();

    _M_create_floor();
    _M_generate_objects(num_objects, workload);
}


auto Scene::handle_events() -> void {
    for (auto& obj : objects) {
        obj->handle_events();
    }

    player.handle_events();
}

// update order
// range -> player
auto Scene::update(f64 delta) -> void {
    update_range(0, objects.size(), delta);
    update_player(delta);
}

// update objects
auto Scene::update_range(usize first, usize last, f64 delta) -> void {
    // (?)
    for (auto i : std::views::iota(first, last)) {
        objects[i]->update(delta);
    }
}

// update the player position
auto Scene::update_player(f64 delta) -> void {
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


auto Scene::_M_create_floor() -> void {
    auto transform = Transform{
        .translation = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion{0.0f, 0.0f, 0.0f, 0.0f},
        .scale = Vector3{.x = 1000.0f, .y = 0.001f, .z = 1000.0f}
    };

    auto floor = std::make_unique<Cube>(transform);
    floor->set_gravity(false);
    floor->set_body_type(rp3d::BodyType::STATIC);
    objects.push_back(std::move(floor));
}


auto Scene::_M_generate_objects(usize num_objects, [[maybe_unused]] Workload workload) -> void {
    auto rand = std::mt19937(SCENE_SEED); // not random_egnine to make things reproducable

    // separate stream
    // keeps cube positions the same at every knob setting
    [[maybe_unused]] auto work_rand = std::mt19937(SCENE_SEED + 1);
    [[maybe_unused]] auto is_heavy = std::bernoulli_distribution(0.1);

    // TODO
    // 10% heavy, 90% light
    // mean stays at magnitude whatever the variance
    //
    //   light = magnitude * (1 - variance)
    //   heavy = magnitude * (1 - variance) + 10 * magnitude * variance
    //
    // check: 0.1 * heavy + 0.9 * light == magnitude
    //
    // otherwise variance changes total work too
    // and a slowdown cant be blamed on imbalance
    [[maybe_unused]] const auto light = u32{0};
    [[maybe_unused]] const auto heavy = u32{0};
    auto xdist = std::uniform_real_distribution<float>(-100.0f, 100.0f);
    auto ydist = std::uniform_real_distribution<float>(-100.0f, 100.0f);
    auto zdist = std::uniform_real_distribution<float>(-50.0f, -30.0f);

    for (auto _ : std::views::iota(num_objects) | std::views::take(num_objects)) {
        auto translation = Vector3{.x = xdist(rand), .y = ydist(rand), .z = zdist(rand)};

        auto transform = Transform{
            .translation = translation,
            .rotation = Quaternion{0.0f, 0.0f, 0.0f, 0.0f},
            .scale = Vector3{1.0f, 1.0f, 1.0f}
        };

        auto cube = std::make_unique<Cube>(transform);

        // TODO
        // cube->work_iterations = is_heavy(work_rand) ? heavy : light;

        objects.push_back(std::move(cube));
    }
}

} // namespace arcxel
