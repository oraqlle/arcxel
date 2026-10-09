#include "scene.h"
#include "cube.h"
#include "game_object.h"
#include "plane.h"
#include "player.h"
#include "sphere.h"
#include "types.h"

#include <raylib.h>
#include <raymath.h>

#include <cassert>
#include <random>
#include <ranges>

namespace arcxel {

static constexpr u32 SCENE_SEED = 20260913;


Scene::Scene() noexcept
: Scene(1) { }


Scene::Scene(usize num_objects, const Vector3 size, Workload workload) noexcept
    : world_size(size) {

    assert(size.x > 0.0f);
    assert(size.y > 0.0f);
    assert(size.z > 0.0f);

    objects.reserve(BASE_OBJ_COUNT + num_objects);

    //_M_create_floor(Vector2{ size.x, size.z });
    //_M_create_walls(size);
    _M_create_scene();
    _M_generate_objects(num_objects, size, workload);
    objects.push_back(std::make_unique<Player>());
}


auto Scene::update(f64 delta) -> void {
    update_range(0, objects.size(), delta);
}


auto Scene::update_range(usize first, usize last, f64 delta) -> void {
    for (auto idx : std::views::iota(first, last)) {
        objects[idx]->update(delta);
    }
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
}

[[nodiscard]] auto Scene::primary_camera() -> Camera3D& {
    return dynamic_cast<Player*>(objects.back().get())->get_camera();
}


auto Scene::unload() -> void { objects.clear(); }


/**
 * @brief Creates a box of various levels which game objects fall through, similar to a "falling sand/oil toy".
 *        The box has ramps or inclines, spinning rotors and different pathways to for objects to fall through.
 *        Objects spawn in a containing box or funnel that "feeds" them into the box with a few different
 *        exists for objects to fall out of. Below this box is another to collect every object that falls.
 *        Front face/plane of box is transparent to allow the user to see inside the box. The box is a 3D.
 */
auto Scene::_M_create_scene() -> void {
    const auto width = world_size.x;
    const auto height = world_size.y;
    const auto depth = world_size.z;

    // ---- FEEDER FUNNEL ----
    const auto feeder_left_transform = Transform{
        .translation = Vector3{ -width * 0.18f, height * 0.88f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 25.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto feeder_left = std::make_unique<Plane>(width * 0.38f, depth * 0.68f, feeder_left_transform);
    objects.push_back(std::move(feeder_left));

    const auto feeder_right_transform = Transform{
        .translation = Vector3{ width * 0.18f, height * 0.88f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, -25.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto feeder_right = std::make_unique<Plane>(width * 0.38f, depth * 0.68f, feeder_right_transform);
    objects.push_back(std::move(feeder_right));

    // ---- UPPER LEVEL ----
    const auto upper_ramp_transform = Transform{
        .translation = Vector3{ 0.0f, height * 0.68f, -depth * 0.12f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, -12.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto upper_ramp = std::make_unique<Plane>(width * 0.72f, depth * 0.42f, upper_ramp_transform);
    objects.push_back(std::move(upper_ramp));

    const auto upper_rotor_transform = Transform{
        .translation = Vector3{ width * 0.16f, height * 0.60f, depth * 0.10f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitY, 35.0f * DEG2RAD),
        .scale = Vector3{ width * 0.14f, height * 0.025f, depth * 0.06f }
    };
    auto upper_rotor = std::make_unique<Cube>(upper_rotor_transform, DARKGRAY);
    objects.push_back(std::move(upper_rotor));

    // ---- MIDDLE LEVEL AND ALTERNATING PATHWAYS ----
    const auto middle_ramp_left_transform = Transform{
        .translation = Vector3{ -width * 0.18f, height * 0.43f, -depth * 0.18f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 15.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto middle_ramp_left = std::make_unique<Plane>(width * 0.48f, depth * 0.36f, middle_ramp_left_transform);
    objects.push_back(std::move(middle_ramp_left));

    const auto middle_ramp_right_transform = Transform{
        .translation = Vector3{ width * 0.18f, height * 0.43f, depth * 0.18f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, -15.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto middle_ramp_right = std::make_unique<Plane>(width * 0.48f, depth * 0.36f, middle_ramp_right_transform);
    objects.push_back(std::move(middle_ramp_right));

    const auto middle_rotor_transform = Transform{
        .translation = Vector3{ -width * 0.12f, height * 0.34f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitY, -25.0f * DEG2RAD),
        .scale = Vector3{ width * 0.14f, height * 0.025f, depth * 0.06f }
    };
    auto middle_rotor = std::make_unique<Cube>(middle_rotor_transform, DARKGRAY);
    objects.push_back(std::move(middle_rotor));

    // ---- LOWER EXITS ----
    const auto exit_left_transform = Transform{
        .translation = Vector3{ -width * 0.24f, height * 0.20f, -depth * 0.20f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, -10.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto exit_left = std::make_unique<Plane>(width * 0.34f, depth * 0.30f, exit_left_transform);
    objects.push_back(std::move(exit_left));

    const auto exit_center_transform = Transform{
        .translation = Vector3{ 0.0f, height * 0.20f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 8.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto exit_center = std::make_unique<Plane>(width * 0.30f, depth * 0.30f, exit_center_transform);
    objects.push_back(std::move(exit_center));

    const auto exit_right_transform = Transform{
        .translation = Vector3{ width * 0.24f, height * 0.20f, depth * 0.20f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 10.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto exit_right = std::make_unique<Plane>(width * 0.34f, depth * 0.30f, exit_right_transform);
    objects.push_back(std::move(exit_right));

    // ---- LOWER COLLECTION BOX ----
    const auto collector_floor_transform = Transform{
        .translation = Vector3{ 0.0f, height * 0.035f, 0.0f },
        .rotation = QuaternionUnitX,
        .scale = Vector3Ones
    };
    auto collector_floor = std::make_unique<Plane>(width * 0.82f, depth * 0.82f, collector_floor_transform);
    objects.push_back(std::move(collector_floor));

    const auto collector_wall_left_transform = Transform{
        .translation = Vector3{ -width * 0.41f, height * 0.10f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto collector_wall_left = std::make_unique<Plane>(height * 0.14f, depth * 0.82f, collector_wall_left_transform);
    objects.push_back(std::move(collector_wall_left));

    const auto collector_wall_right_transform = Transform{
        .translation = Vector3{ width * 0.41f, height * 0.10f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto collector_wall_right = std::make_unique<Plane>(height * 0.14f, depth * 0.82f, collector_wall_right_transform);
    objects.push_back(std::move(collector_wall_right));

    const auto collector_wall_back_transform = Transform{
        .translation = Vector3{ 0.0f, height * 0.10f, -depth * 0.41f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 90.0f * DEG2RAD),
        .scale = Vector3Ones
    };
    auto collector_wall_back = std::make_unique<Plane>(width * 0.82f, height * 0.14f, collector_wall_back_transform);
    objects.push_back(std::move(collector_wall_back));
}


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


auto Scene::_M_generate_objects(
    usize num_objects,
    const Vector3 size,
    [[maybe_unused]] Workload workload) -> void {
    const auto xdim = size.x * 0.5f;
    const auto zdim = size.z * 0.5f;

    auto rand = std::mt19937(SCENE_SEED);
    auto xdist = std::uniform_real_distribution<f32>(-xdim, xdim);
    auto ydist = std::uniform_real_distribution<f32>(10.0f, size.y);
    auto zdist = std::uniform_real_distribution<f32>(-zdim, zdim);
    auto shape_type_dist = std::uniform_int_distribution<u32>{};

    [[maybe_unused]] auto work_rand = std::mt19937(SCENE_SEED + 1);
    [[maybe_unused]] auto is_heavy = std::bernoulli_distribution(0.1);

    [[maybe_unused]] const auto light = (double)workload.magnitude * (1.0 - workload.variance);
    [[maybe_unused]] const auto heavy = (double)workload.magnitude * (1.0 - workload.variance) + 10.0 * (double)workload.magnitude * workload.variance;

    assert((0.1 * heavy) + (0.9 * light) == workload.magnitude);

    for ([[maybe_unused]] auto _ : std::views::iota(usize{ 0 }, num_objects)) {
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

		objects.back()->work_iterations = static_cast<u32>(is_heavy(work_rand) ? heavy : light);
    }
}

} // namespace arcxel
