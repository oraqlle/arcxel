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

Scene::Scene() noexcept
: Scene(1) { }


Scene::Scene(usize num_objects, Workload workload) noexcept {

    objects.reserve(BASE_OBJ_COUNT + num_objects);

    _M_create_maze();
    _M_create_catchment_box();
    _M_generate_objects(num_objects, workload);
    _M_generate_transparent_face(); //< Created last as transparent objects must be rendered back-to-front
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
    const auto rot_z_90 = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD);
    const auto rot_y_90 = QuaternionFromAxisAngle(Vector3UnitY, 90 * DEG2RAD);
    const auto rot_y_neg90 = QuaternionFromAxisAngle(Vector3UnitY, -90 * DEG2RAD);

    // --- BACK WALL ---
    const auto back_pos = Vector3{ .x = -MAZE_DEPTH * 0.5f , .y = MAZE_CENTRE_YPOS, .z = 0.0f };
    const auto back_rot = rot_z_90;
    _M_add_plane(MAZE_HEIGHT, MAZE_WIDTH, back_pos, back_rot, CONTAINER_COLOUR);


    // --- LEFT WALL ----
    const auto left_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS, .z = -MAZE_WIDTH * 0.5f };
    const auto left_rot = QuaternionMultiply(rot_y_90, rot_z_90);
    _M_add_plane(MAZE_HEIGHT, MAZE_DEPTH, left_pos, left_rot, CONTAINER_COLOUR);


    // --- RIGHT WALL ---
    const auto right_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS, .z = MAZE_WIDTH * 0.5f };
    const auto right_rot = QuaternionMultiply(rot_y_neg90, rot_z_90);
    _M_add_plane(MAZE_HEIGHT, MAZE_DEPTH, right_pos, right_rot, CONTAINER_COLOUR);


    // ---- CREATE RAMPS ----
    const auto r1_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS + 40.0f, .z = 19.0f };
    const auto r1_tilt = QuaternionFromAxisAngle(Vector3UnitX, -18.0f * DEG2RAD);
    const auto r1_rot = QuaternionMultiply(r1_tilt, rot_y_90);
    const auto r1_length = 17.0f;
    _M_add_plane(r1_length, MAZE_DEPTH, r1_pos, r1_rot, BLUE);

    const auto r2_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS + 35.0f, .z = -15.0f };
    const auto r2_tilt = QuaternionFromAxisAngle(Vector3UnitX, 15.0f * DEG2RAD);
    const auto r2_rot = QuaternionMultiply(r2_tilt, rot_y_90);
    const auto r2_length = 20.0f;
    _M_add_plane(r2_length, MAZE_DEPTH, r2_pos, r2_rot, GREEN);

    const auto r3_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS - 12.0f, .z = 9.0f };
    const auto r3_tilt = QuaternionFromAxisAngle(Vector3UnitX, 20.0f * DEG2RAD);
    const auto r3_rot = QuaternionMultiply(r3_tilt, rot_y_90);
    const auto r3_length = 19.0f;
    _M_add_plane(r3_length, MAZE_DEPTH, r3_pos, r3_rot, MAGENTA);

    const auto r4_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS + 5.0f, .z = 20.0f };
    const auto r4_tilt = QuaternionFromAxisAngle(Vector3UnitX, -30.0f * DEG2RAD);
    const auto r4_rot = QuaternionMultiply(r4_tilt, rot_y_90);
    const auto r4_length = 10.0f;
    _M_add_plane(r4_length, MAZE_DEPTH, r4_pos, r4_rot, ORANGE);

    const auto r5_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS - 23.0f, .z = -8.0f };
    const auto r5_tilt = QuaternionFromAxisAngle(Vector3UnitX, -17.0f * DEG2RAD);
    const auto r5_rot = QuaternionMultiply(r5_tilt, rot_y_90);
    const auto r5_length = 22.0f;
    _M_add_plane(r5_length, MAZE_DEPTH, r5_pos, r5_rot, PURPLE);

    const auto r6_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS + 10.0f, .z = -20.0f };
    const auto r6_tilt = QuaternionFromAxisAngle(Vector3UnitX, 71.0f * DEG2RAD);
    const auto r6_rot = QuaternionMultiply(r6_tilt, rot_y_90);
    const auto r6_length = 21.0f;
    _M_add_plane(r6_length, MAZE_DEPTH, r6_pos, r6_rot, PINK);

    const auto r7_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS + 20.0f, .z = -5.0f };
    const auto r7_tilt = QuaternionFromAxisAngle(Vector3UnitX, -25.0f * DEG2RAD);
    const auto r7_rot = QuaternionMultiply(r7_tilt, rot_y_90);
    const auto r7_length = 14.0f;
    _M_add_plane(r7_length, MAZE_DEPTH, r7_pos, r7_rot, SKYBLUE);

    const auto r8_pos = Vector3{ .x = 0.0f , .y = MAZE_CENTRE_YPOS + 20.0f, .z = 10.0f };
    const auto r8_tilt = QuaternionFromAxisAngle(Vector3UnitX, 25.0f * DEG2RAD);
    const auto r8_rot = QuaternionMultiply(r8_tilt, rot_y_90);
    const auto r8_length = 14.0f;
    _M_add_plane(r8_length, MAZE_DEPTH, r8_pos, r8_rot, YELLOW);


    // --- SINKS ---
    const auto sinks_ypos = MAZE_CENTRE_YPOS - (MAZE_HEIGHT * 0.5f) + 5.0f;
    const auto sinks_length = 0.45f * MAZE_WIDTH;
    const auto sinks_zpos = 0.29f * MAZE_WIDTH;

    const auto r9_pos = Vector3{ .x = 0.0f , .y = sinks_ypos, .z = -sinks_zpos };
    const auto r9_tilt = QuaternionFromAxisAngle(Vector3UnitX, 20.0f * DEG2RAD);
    const auto r9_rot = QuaternionMultiply(r9_tilt, rot_y_90);
    _M_add_plane(sinks_length, MAZE_DEPTH, r9_pos, r9_rot, RED);

    const auto r10_pos = Vector3{ .x = 0.0f , .y = sinks_ypos, .z = sinks_zpos };
    const auto r10_tilt = QuaternionFromAxisAngle(Vector3UnitX, -20.0f * DEG2RAD);
    const auto r10_rot = QuaternionMultiply(r10_tilt, rot_y_90);
    _M_add_plane(sinks_length, MAZE_DEPTH, r10_pos, r10_rot, RED);
}


auto Scene::_M_generate_transparent_face() -> void {
    const auto position = Vector3{ .x = MAZE_DEPTH * 0.5f, .y = MAZE_CENTRE_YPOS, .z = 0.0f };
    const auto rotation = QuaternionFromAxisAngle(Vector3UnitZ, -90 * DEG2RAD);
    _M_add_plane(
        MAZE_HEIGHT,
        MAZE_WIDTH,
        position,
        rotation,
        Fade(BLACK, 0.50f)); //< BLACK colour, 50% opaque
}


auto Scene::_M_create_catchment_box() -> void {
    // ---- FLOOR ----
    auto floor = std::make_unique<Plane>(BOX_SIZE.x, BOX_SIZE.z);
    objects.push_back(std::move(floor));

    // ---- LEFT WALL ----
    const auto left_transform = Transform{
        .translation = Vector3{ 0.0f, BOX_SIZE.y * 0.5f, BOX_SIZE.z * 0.5f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto left = std::make_unique<Plane>(BOX_SIZE.x, BOX_SIZE.y, left_transform);
    objects.push_back(std::move(left));

    // ---- RIGHT WALL ----
    const auto right_transform = Transform{
        .translation = Vector3{ 0.0f, BOX_SIZE.y * 0.5f, BOX_SIZE.z * -0.5f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitX, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto right = std::make_unique<Plane>(BOX_SIZE.x, BOX_SIZE.y, right_transform);
    objects.push_back(std::move(right));

    // ---- TOP WALL ----
    const auto top_transform = Transform{
        .translation = Vector3{ BOX_SIZE.x * -0.5f, BOX_SIZE.y * 0.5f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto top = std::make_unique<Plane>(BOX_SIZE.y, BOX_SIZE.z, top_transform);
    objects.push_back(std::move(top));

    // ---- BOTTOM WALL ----
    const auto bottom_transform = Transform{
        .translation = Vector3{ BOX_SIZE.x * 0.5f, BOX_SIZE.y * 0.5f, 0.0f },
        .rotation = QuaternionFromAxisAngle(Vector3UnitZ, 90 * DEG2RAD),
        .scale = Vector3Ones
    };

    auto bottom = std::make_unique<Plane>(BOX_SIZE.y, BOX_SIZE.z, bottom_transform);
    objects.push_back(std::move(bottom));
}


auto Scene::_M_generate_objects(usize num_objects, Workload workload) -> void {
    const auto xdim = MAZE_DEPTH * 0.5f * 0.29f;
    const auto ydim_low = MAZE_CENTRE_YPOS + (MAZE_HEIGHT * 0.5f) + 50.0f;
    const auto ydim_high = MAZE_CENTRE_YPOS + (MAZE_HEIGHT * 0.5f) + 350.0f;
    const auto zdim = MAZE_WIDTH * 0.5f * 0.88f;

    auto rd = std::random_device{};
    auto rand = std::mt19937(rd());
    auto xdist = std::uniform_real_distribution<f32>(-xdim, xdim);
    auto ydist = std::uniform_real_distribution<f32>(ydim_low, ydim_high);
    auto zdist = std::uniform_real_distribution<f32>(-zdim, zdim);
    auto shape_type_dist = std::uniform_int_distribution<u32>{};

    auto work_rand = std::mt19937(rd());
    auto is_heavy = std::bernoulli_distribution(0.1);

    const auto mag = static_cast<f64>(workload.magnitude);
    const auto inv_var = 1.0 - workload.variance;
    const auto light = mag * inv_var;
    const auto heavy = mag * inv_var + 10.0 * mag * workload.variance;

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
            .g = static_cast<unsigned char>(std::abs(translation.y / BOX_SIZE.y) * 255.0f),
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
