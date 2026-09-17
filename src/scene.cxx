#include "scene.h"
#include "cube.h"
#include "game_object.h"
#include "plane.h"
#include "player.h"
#include "sphere.h"
#include "types.h"

#include <cassert>
#include <raylib.h>
#include <raymath.h>

#include <random>
#include <ranges>

namespace arcxel {

Scene::Scene() noexcept {
    auto player = Player();

    _M_create_floor(Vector2{ DEFAULT_BOX_SIZE.x, DEFAULT_BOX_SIZE.z });
    _M_create_walls(DEFAULT_BOX_SIZE);
    _M_generate_objects(1, DEFAULT_BOX_SIZE);
}


Scene::Scene(usize num_objects, const Vector3 size) noexcept
    : world_size(size) {

    assert(size.x > 0.0f);
    assert(size.y > 0.0f);
    assert(size.z > 0.0f);

    auto player = Player();

    _M_create_floor(Vector2{ size.x, size.z });
    _M_create_walls(size);
    _M_generate_objects(num_objects, size);
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

    auto x_axis = Vector3{ 1000.0f, 0.0f, 0.0f };
    auto y_axis = Vector3{ 0.0f, 1000.0f, 0.0f };
    auto z_axis = Vector3{ 0.0f, 0.0f, 1000.0f };

    DrawLine3D(x_axis, Vector3Zeros - x_axis, RED);
    DrawLine3D(y_axis, Vector3Zeros - y_axis, GREEN);
    DrawLine3D(z_axis, Vector3Zeros - z_axis, BLUE);


    for (auto& obj : objects) {
        obj->render(delta);
    }

    player.render(delta);
}


[[nodiscard]] auto Scene::primary_camera() -> Camera3D { return player.get_camera(); }


auto Scene::unload() -> void { objects.clear(); }


auto Scene::_M_create_floor(const Vector2 size) -> void {
    auto floor = std::make_unique<Plane>(size.x, size.y);
    objects.push_back(std::move(floor));
}


auto Scene::_M_create_walls(const Vector3 size) -> void {
    // ---- LEFT WALL ----
    const auto left_transform = Transform{
        .translation = Vector3{ 0.0f, size.y * 0.5f, size.z * 0.5f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto left = std::make_unique<Plane>(size.x, size.y, left_transform);
    objects.push_back(std::move(left));

    // ---- RIGHT WALL ----
    const auto right_transform = Transform{
        .translation = Vector3{ 0.0f, size.y * 0.5f, size.z * -0.5f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto right = std::make_unique<Plane>(size.x, size.y, right_transform);
    objects.push_back(std::move(right));

    // ---- TOP WALL ----
    const auto top_transform = Transform{
        .translation = Vector3{ size.x * -0.5f, size.y * 0.5f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto top = std::make_unique<Plane>(size.y, size.z, top_transform);
    objects.push_back(std::move(top));

    // ---- BOTTOM WALL ----
    const auto bottom_transform = Transform{
        .translation = Vector3{ size.x * 0.5f, size.y * 0.5f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto bottom = std::make_unique<Plane>(size.y, size.z, bottom_transform);
    objects.push_back(std::move(bottom));
}


auto Scene::_M_generate_objects(usize num_objects, const Vector3 size) -> void {
    const auto xdim = size.x * 0.5f;
    const auto zdim = size.z * 0.5f;

    auto rand = std::default_random_engine(std::random_device{}());
    auto xdist = std::uniform_real_distribution<f32>(-xdim, xdim);
    auto ydist = std::uniform_real_distribution<f32>(10.0f, size.y);
    auto zdist = std::uniform_real_distribution<f32>(-zdim, zdim);
    auto shape_type_dist = std::uniform_int_distribution<u32>{};

    for (auto _ : std::views::iota(num_objects) | std::views::take(num_objects)) {
        auto translation = Vector3{ .x = xdist(rand),
                                    .y = ydist(rand),
                                    .z = zdist(rand) };

        auto transform = Transform{ .translation = translation,
                                    .rotation = QuaternionUnitX,
                                    .scale = Vector3Ones };

        auto colour = Color{
            .r = static_cast<unsigned char>(std::abs(translation.x / xdim) * 255.0f),
            .g = static_cast<unsigned char>(std::abs(translation.y / size.y) * 255.0f),
            .b = static_cast<unsigned char>(std::abs(translation.z / zdim) * 255.0f),
            .a = 255
        };

        switch (shape_type_dist(rand) % 2) {
            case 0: // Cube
                objects.push_back(std::make_unique<Cube>(transform, colour));
                break;

            case 1: // Sphere
                objects.push_back(std::make_unique<Sphere>(transform, colour));
                break;
        }
    }
}

} // namespace arcxel
