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

    //_M_create_funnel();
    _M_create_maze();
    _M_create_catchment_box();
    _M_generate_objects(num_objects, workload);
    _M_generate_transparent_face();
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


auto Scene::_M_add_plane(
    f32 length,
    f32 width,
    Vector3 position,
    Quaternion rotation,
    Color tint) -> void {

    const auto transform = Transform{
        .translation = position,
        .rotation = rotation,
        .scale = Vector3Ones
    };

    objects.push_back(std::make_unique<Plane>(length, width, transform, tint));
}


auto Scene::_M_create_maze() -> void {

    // ---- CREATE CONTAINING BOX ----
    constexpr auto ypos = 100.0f;
    constexpr auto height = 75.0f;
    constexpr auto width = 50.0f;
    constexpr auto depth = 7.0f;

    constexpr auto CONTAINER_COLOUR = GRAY;

    const auto rot_z_90 = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD);
    const auto rot_y_90 = QuaternionFromAxisAngle(Vector3UnitY, 90 * DEG2RAD);
    const auto rot_y_neg90 = QuaternionFromAxisAngle(Vector3UnitY, -90 * DEG2RAD);

    // --- BACK WALL ---
    const auto back_pos = Vector3{ .x = -depth / 2.0f , .y = ypos, .z = 0.0f };
    const auto back_rot = rot_z_90;
    _M_add_plane(height, width, back_pos, back_rot, CONTAINER_COLOUR);


    // --- LEFT WALL ----
    const auto left_pos = Vector3{ .x = 0.0f , .y = ypos, .z = -width / 2.0f };
    const auto left_rot = QuaternionMultiply(rot_y_90, rot_z_90);
    _M_add_plane(height, depth, left_pos, left_rot, CONTAINER_COLOUR);


    // --- RIGHT WALL ---
    const auto right_pos = Vector3{ .x = 0.0f , .y = ypos, .z = width / 2.0f };
    const auto right_rot = QuaternionMultiply(rot_y_neg90, rot_z_90);
    _M_add_plane(height, depth, right_pos, right_rot, CONTAINER_COLOUR);


    // ---- CREATE RAMPS ----
    const auto r1_pos = Vector3{ .x = 0.0f , .y = ypos + 10.0f, .z = 5.0f };
    const auto r1_tilt = QuaternionFromAxisAngle(Vector3UnitX, 10.0f * DEG2RAD);
    const auto r1_rot = QuaternionMultiply(r1_tilt, rot_y_90);
    const auto r1_length = 17.0f;
    _M_add_plane(r1_length, depth, r1_pos, r1_rot, BLUE);

    const auto r2_pos = Vector3{ .x = 0.0f , .y = ypos + 35.0f, .z = -15.0f };
    const auto r2_tilt = QuaternionFromAxisAngle(Vector3UnitX, 15.0f * DEG2RAD);
    const auto r2_rot = QuaternionMultiply(r2_tilt, rot_y_90);
    const auto r2_length = 20.0f;
    _M_add_plane(r2_length, depth, r2_pos, r2_rot, GREEN);

    const auto r3_pos = Vector3{ .x = 0.0f , .y = ypos - 12.0f, .z = 9.0f };
    const auto r3_tilt = QuaternionFromAxisAngle(Vector3UnitX, 20.0f * DEG2RAD);
    const auto r3_rot = QuaternionMultiply(r3_tilt, rot_y_90);
    const auto r3_length = 19.0f;
    _M_add_plane(r3_length, depth, r3_pos, r3_rot, MAGENTA);

    const auto r4_pos = Vector3{ .x = 0.0f , .y = ypos + 5.0f, .z = 20.0f };
    const auto r4_tilt = QuaternionFromAxisAngle(Vector3UnitX, -30.0f * DEG2RAD);
    const auto r4_rot = QuaternionMultiply(r4_tilt, rot_y_90);
    const auto r4_length = 10.0f;
    _M_add_plane(r4_length, depth, r4_pos, r4_rot, ORANGE);

    const auto r5_pos = Vector3{ .x = 0.0f , .y = ypos - 23.0f, .z = -8.0f };
    const auto r5_tilt = QuaternionFromAxisAngle(Vector3UnitX, -17.0f * DEG2RAD);
    const auto r5_rot = QuaternionMultiply(r5_tilt, rot_y_90);
    const auto r5_length = 22.0f;
    _M_add_plane(r5_length, depth, r5_pos, r5_rot, PURPLE);

    const auto r6_pos = Vector3{ .x = 0.0f , .y = ypos + 10.0f, .z = -16.0f };
    const auto r6_tilt = QuaternionFromAxisAngle(Vector3UnitX, 41.0f * DEG2RAD);
    const auto r6_rot = QuaternionMultiply(r6_tilt, rot_y_90);
    const auto r6_length = 21.0f;
    _M_add_plane(r6_length, depth, r6_pos, r6_rot, PINK);

    const auto r7_pos = Vector3{ .x = 0.0f , .y = ypos + 20.0f, .z = -5.0f };
    const auto r7_tilt = QuaternionFromAxisAngle(Vector3UnitX, -25.0f * DEG2RAD);
    const auto r7_rot = QuaternionMultiply(r7_tilt, rot_y_90);
    const auto r7_length = 14.0f;
    _M_add_plane(r7_length, depth, r7_pos, r7_rot, SKYBLUE);

    const auto r8_pos = Vector3{ .x = 0.0f , .y = ypos + 20.0f, .z = 10.0f };
    const auto r8_tilt = QuaternionFromAxisAngle(Vector3UnitX, 25.0f * DEG2RAD);
    const auto r8_rot = QuaternionMultiply(r8_tilt, rot_y_90);
    const auto r8_length = 14.0f;
    _M_add_plane(r8_length, depth, r8_pos, r8_rot, YELLOW);


    // --- SINKS ---
    const auto sinks_ypos = ypos - (height / 2.0f) - 4.0f;
    const auto sinks_length = 0.48f * width;
    const auto sinks_zpos = 0.28f * width;

    const auto r9_pos = Vector3{ .x = 0.0f , .y = sinks_ypos, .z = -sinks_zpos };
    const auto r9_tilt = QuaternionFromAxisAngle(Vector3UnitX, 20.0f * DEG2RAD);
    const auto r9_rot = QuaternionMultiply(r9_tilt, rot_y_90);
    _M_add_plane(sinks_length, depth, r9_pos, r9_rot, RED);

    const auto r10_pos = Vector3{ .x = 0.0f , .y = sinks_ypos, .z = sinks_zpos };
    const auto r10_tilt = QuaternionFromAxisAngle(Vector3UnitX, -20.0f * DEG2RAD);
    const auto r10_rot = QuaternionMultiply(r10_tilt, rot_y_90);
    _M_add_plane(sinks_length, depth, r10_pos, r10_rot, RED);
}


auto Scene::_M_generate_transparent_face() -> void {
    const auto height = 100.0f;
    const auto width = 50.0f;
    const auto depth = 7.0f;
    const auto position = Vector3{ .x = depth / 2 , .y = 100.0f, .z = 0.0f };
    const auto rotation = QuaternionFromAxisAngle(Vector3UnitZ, -90 * DEG2RAD);
    _M_add_plane(
        height,
        width,
        position,
        rotation,
        Fade(BLACK, 0.50f)); //< BLACK colour, 50% opaque
}


auto Scene::_M_create_catchment_box() -> void {
    // ---- FLOOR ----
    auto floor = std::make_unique<Plane>(world_size.x, world_size.z);
    objects.push_back(std::move(floor));

    // ---- LEFT WALL ----
    const auto left_transform = Transform{
        .translation = Vector3{ 0.0f, world_size.y * 0.5f, world_size.z * 0.5f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto left = std::make_unique<Plane>(world_size.x, world_size.y, left_transform);
    objects.push_back(std::move(left));

    // ---- RIGHT WALL ----
    const auto right_transform = Transform{
        .translation = Vector3{ 0.0f, world_size.y * 0.5f, world_size.z * -0.5f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto right = std::make_unique<Plane>(world_size.x, world_size.y, right_transform);
    objects.push_back(std::move(right));

    // ---- TOP WALL ----
    const auto top_transform = Transform{
        .translation = Vector3{ world_size.x * -0.5f, world_size.y * 0.5f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto top = std::make_unique<Plane>(world_size.y, world_size.z, top_transform);
    objects.push_back(std::move(top));

    // ---- BOTTOM WALL ----
    const auto bottom_transform = Transform{
        .translation = Vector3{ world_size.x * 0.5f, world_size.y * 0.5f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto bottom = std::make_unique<Plane>(world_size.y, world_size.z, bottom_transform);
    objects.push_back(std::move(bottom));
}


auto Scene::_M_generate_objects(
    usize num_objects,
    [[maybe_unused]] Workload workload) -> void {
    const auto xdim = 4.0f;
    const auto zdim = 22.0f;

    auto rand = std::mt19937(SCENE_SEED);
    auto xdist = std::uniform_real_distribution<f32>(-xdim, xdim);
    auto ydist = std::uniform_real_distribution<f32>(200.0f, 500.0f);
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
            .g = static_cast<unsigned char>(std::abs(translation.y / world_size.y) * 255.0f),
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
