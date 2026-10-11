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


auto _M_create_funnel() -> void {
    // ---- LEFT SLOPE ----
    //_M_add_plane();

    // ---- LEFT UPPER SIDE ----


    // ---- LEFT LOWER SIDE ----


    // ---- RIGHT SLOPE ----


    // ---- RIGHT UPPER SIDE ----


    // ---- RIGHT LOWER SIDE ----


    // ---- FRONT SLOPE ----


    // ---- FRONT UPPER SIDE ----


    // ---- FRONT LOWER SIDE ----


    // ---- BACK SLOPE ----


    // ---- BACK UPPER SIDE ----


    // ---- BACK LOWER SIDE ----


    //const Quaternion horizontal =
    //    QuaternionFromAxisAngle(Vector3UnitY, 0.0f);

    //// Rotates the default XZ plane into an XY plane.
    //const Quaternion backWallRotation = QuaternionFromAxisAngle(Vector3UnitX, 90.0f * DEG2RAD);

    //// Rotates the default XZ plane into a YZ plane.
    //const Quaternion sideWallRotation =
    //    QuaternionFromAxisAngle(Vector3UnitZ, 90.0f * DEG2RAD);

    //// Chamber walls. The front is mostly transparent, with an open strip
    //// at the bottom so exits can feed into the collection bin.
    //addPlane(W, H,
    //         Vector3{ W * 0.5f, H * 0.5f, D },
    //         backWallRotation, frameColor);

    //addPlane(H, D,
    //         Vector3{ 0.0f, H * 0.5f, D * 0.5f },
    //         sideWallRotation, frameColor);

    //addPlane(H, D,
    //         Vector3{ W, H * 0.5f, D * 0.5f },
    //         sideWallRotation, frameColor);

    //addPlane(W, D,
    //         Vector3{ W * 0.5f, H, D * 0.5f },
    //         horizontal, frameColor);

    //addPlane(W, D,
    //         Vector3{ W * 0.5f, 0.0f, D * 0.5f },
    //         horizontal, frameColor);

    //// Transparent front panel. It begins above the exit level, leaving
    //// the bottom openings unobstructed. Alpha transparency also depends
    //// on the renderer's blending/material settings.
    //constexpr f32 frontPanelBottom = 2.5f;
    //const f32 frontPanelHeight = H - frontPanelBottom;
    //const auto tuv = Vector3RotateByQuaternion(Vector3UnitY, backWallRotation);
    //const auto xyz = QuaternionFromAxisAngle(Vector3UnitZ, 80 * DEG2RAD);
    //const auto frontRot = QuaternionFromAxisAngle(Vector3UnitX, -90 * DEG2RAD);

    //addPlane(W, frontPanelHeight,
    //         Vector3{ W * 0.5f,
    //                  frontPanelBottom + frontPanelHeight * 0.5f,
    //                  0.0f },
    //         frontRot, glassColor);

    //// Hopper sides and a broad entry shelf.
    //// Their different slopes help spread objects into separate routes.
    //addPlane(7.0f, D - 1.0f,
    //         Vector3{ 8.0f, 25.0f, D * 0.5f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, -28.0f * DEG2RAD),
    //         rampA);

    //addPlane(7.0f, D - 1.0f,
    //         Vector3{ 16.0f, 25.0f, D * 0.5f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 28.0f * DEG2RAD),
    //         rampA);

    //addPlane(8.0f, 4.0f,
    //         Vector3{ 12.0f, 22.2f, 3.5f },
    //         horizontal, rampB);

    //// Long, staggered ramps. Their varied depths, angles, and gaps form
    //// a branching maze instead of one continuous back-and-forth path.
    //addPlane(7.0f, 3.5f,
    //         Vector3{ 5.0f, 19.5f, 1.8f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, -18.0f * DEG2RAD),
    //         rampA);

    //addPlane(6.5f, 3.5f,
    //         Vector3{ 18.0f, 18.6f, 5.8f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 22.0f * DEG2RAD),
    //         rampB);

    //addPlane(6.0f, 3.0f,
    //         Vector3{ 11.0f, 16.5f, 4.0f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 12.0f * DEG2RAD),
    //         rampC);

    //addPlane(7.0f, 3.5f,
    //         Vector3{ 18.5f, 13.9f, 1.8f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, -24.0f * DEG2RAD),
    //         rampA);

    //addPlane(7.0f, 3.5f,
    //         Vector3{ 5.0f, 12.5f, 5.9f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 20.0f * DEG2RAD),
    //         rampB);

    //addPlane(6.0f, 3.0f,
    //         Vector3{ 12.5f, 10.5f, 3.7f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, -10.0f * DEG2RAD),
    //         rampC);

    //addPlane(7.0f, 3.5f,
    //         Vector3{ 18.0f, 8.3f, 5.7f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 18.0f * DEG2RAD),
    //         rampA);

    //addPlane(7.0f, 3.5f,
    //         Vector3{ 5.0f, 6.8f, 1.7f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, -16.0f * DEG2RAD),
    //         rampB);

    //addPlane(6.0f, 3.0f,
    //         Vector3{ 12.0f, 4.8f, 4.0f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 12.0f * DEG2RAD),
    //         rampC);

    //// Short branch/reconnect ledges, separated from the main ramps.
    //addPlane(3.5f, 2.5f,
    //         Vector3{ 11.0f, 19.7f, 6.5f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 10.0f * DEG2RAD),
    //         rampC);

    //addPlane(3.5f, 2.5f,
    //         Vector3{ 7.0f, 14.2f, 3.7f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, -12.0f * DEG2RAD),
    //         rampA);

    //addPlane(3.5f, 2.5f,
    //         Vector3{ 15.5f, 7.8f, 2.5f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 14.0f * DEG2RAD),
    //         rampB);

    //// Static crossed-blade rotor geometry. Rotate these objects in your
    //// update loop if you want them to spin during play.
    //auto addRotor = [&](Vector3 center, f32 bladeLength)
    //{
    //    addPlane(bladeLength, 0.45f, center,
    //             QuaternionFromAxisAngle(Vector3UnitY, 35.0f * DEG2RAD),
    //             rotorColor);

    //    addPlane(bladeLength, 0.45f, center,
    //             QuaternionFromAxisAngle(Vector3UnitY, 125.0f * DEG2RAD),
    //             rotorColor);
    //};

    //addRotor(Vector3{ 12.0f, 20.3f, 4.0f }, 3.0f);
    //addRotor(Vector3{ 12.0f, 14.3f, 3.5f }, 3.0f);
    //addRotor(Vector3{ 12.0f, 8.8f, 4.2f }, 3.2f);

    //// Three low exit guides lead toward separate openings.
    //addPlane(3.5f, 3.0f,
    //         Vector3{ 3.0f, 2.0f, 2.0f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, -12.0f * DEG2RAD),
    //         rampA);

    //addPlane(3.5f, 3.0f,
    //         Vector3{ 12.0f, 2.0f, 4.0f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 8.0f * DEG2RAD),
    //         rampB);

    //addPlane(3.5f, 3.0f,
    //         Vector3{ 21.0f, 2.0f, 6.0f },
    //         QuaternionFromAxisAngle(Vector3UnitZ, 15.0f * DEG2RAD),
    //         rampC);

    //// Collection bin below the chamber: floor, front/back walls, and ends.
    //constexpr f32 binWidth = W + 2.0f;
    //constexpr f32 binDepth = D + 1.0f;
    //constexpr f32 binWallHeight = 2.5f;
    //constexpr f32 binCenterY = -2.0f;

    //addPlane(binWidth, binDepth,
    //         Vector3{ W * 0.5f, -3.2f, D * 0.5f },
    //         horizontal, binColor);

    //addPlane(binWidth, binWallHeight,
    //         Vector3{ W * 0.5f, binCenterY, -0.5f },
    //         backWallRotation, binColor);

    //addPlane(binWidth, binWallHeight,
    //         Vector3{ W * 0.5f, binCenterY, D + 0.5f },
    //         backWallRotation, binColor);

    //addPlane(binWallHeight, binDepth,
    //         Vector3{ -1.0f, binCenterY, D * 0.5f },
    //         sideWallRotation, binColor);

    //addPlane(binWallHeight, binDepth,
    //         Vector3{ W + 1.0f, binCenterY, D * 0.5f },
    //         sideWallRotation, binColor);
}


auto Scene::_M_create_maze() -> void {

    // ---- CREATE CONTAINING BOX ----
    constexpr auto ypos = 100.0f;
    constexpr auto height = 75.0f;
    constexpr auto width = 50.0f;
    constexpr auto depth = 4.0f;

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
    const auto r1_length = 10.0f;
    _M_add_plane(r1_length, depth, r1_pos, r1_rot, BLUE);

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


auto Scene::_M_generate_transparent_face() -> void {
    const auto height = 75.0f;
    const auto width = 50.0f;
    const auto depth = 4.0f;
    const auto position = Vector3{ .x = depth / 2 , .y = 100.0f, .z = 0.0f };
    const auto rotation = QuaternionFromAxisAngle(Vector3UnitZ, -90 * DEG2RAD);
    _M_add_plane(
        height,
        width,
        position,
        rotation,
        Fade(BLACK, 0.50f)); //< BLACK colour, 50% opaque
}

auto Scene::_M_generate_objects(
    usize num_objects,
    [[maybe_unused]] Workload workload) -> void {
    const auto xdim = world_size.x * 0.5f;
    const auto zdim = world_size.z * 0.5f;

    auto rand = std::mt19937(SCENE_SEED);
    auto xdist = std::uniform_real_distribution<f32>(-xdim, xdim);
    auto ydist = std::uniform_real_distribution<f32>(10.0f, world_size.y);
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
