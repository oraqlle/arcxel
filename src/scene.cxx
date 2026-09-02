#include "scene.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

namespace arcxel {

namespace {

constexpr auto default_interior = Vector3{10.0f, 6.0f, 10.0f};
constexpr f32 default_wall_thickness = 0.5f;

constexpr f32 default_sphere_radius = 0.5f;
constexpr i32 sphere_rings = 12;
constexpr i32 sphere_slices = 16;

constexpr auto floor_colour = Color{206, 212, 208, 255};
constexpr auto wall_colour = Color{120, 132, 128, 255};
constexpr auto sphere_colour = Color{11, 110, 95, 255};

// rp3d's vector is not a literal type, so these cannot be constexpr.
const auto gravity = rp3d::Vector3(0.0f, -9.81f, 0.0f);

// High and to one side, so the drop meets a wall as well as the floor.
const auto spawn_position = rp3d::Vector3(-4.0f, 9.0f, 3.0f);
const auto spawn_velocity = rp3d::Vector3(4.0f, 0.0f, -2.5f);

constexpr f32 sphere_bounciness = 0.75f;

// rp3d takes half-extents, raylib draws with full ones. Converting in a single
// place is what stops the two descriptions of the container disagreeing.
[[nodiscard]] constexpr auto full_extents(Vector3 half) -> Vector3 {
    return Vector3{half.x * 2.0f, half.y * 2.0f, half.z * 2.0f};
}


[[nodiscard]] auto to_rp3d(Vector3 v) -> rp3d::Vector3 {
    return rp3d::Vector3(v.x, v.y, v.z);
}



// Bridge an rp3d transform into a raylib one.
[[nodiscard]] auto to_raylib(const rp3d::Transform& transform) -> Matrix {
    f32 m[16];
    transform.getOpenGLMatrix(m);

    return Matrix{
        .m0 = m[0], .m4 = m[4], .m8 = m[8],   .m12 = m[12],
        .m1 = m[1], .m5 = m[5], .m9 = m[9],   .m13 = m[13],
        .m2 = m[2], .m6 = m[6], .m10 = m[10], .m14 = m[14],
        .m3 = m[3], .m7 = m[7], .m11 = m[11], .m15 = m[15],
    };
}

} // namespace


Scene::Scene() noexcept
    : interior(default_interior)
    , wall_thickness(default_wall_thickness)
    , sphere_radius(default_sphere_radius) {
    // Sitting on the floor, as a reference for scale until the sphere replaces it.
    cube.transform.position = Vector3{0.0f, 0.5f, -5.0f};

    // Needs a live GL context, so Scene must outlive nothing and the window must
    // outlive it. main.cxx opens the window before Engine is constructed.
    sphere_mesh = GenMeshSphere(sphere_radius, sphere_rings, sphere_slices);
    sphere_material = LoadMaterialDefault();
    sphere_material.maps[MATERIAL_MAP_DIFFUSE].color = sphere_colour;

    auto settings = rp3d::PhysicsWorld::WorldSettings();
    settings.gravity = gravity;

    // Off, or the spheres settle and the workload quietly shrinks mid-run.
    settings.isSleepingEnabled = false;

    world = physics.createPhysicsWorld(settings);
    world->enableSleeping(false);

    _build_container();

    sphere_shape = physics.createSphereShape(sphere_radius);
    sphere = world->createRigidBody(rp3d::Transform::identity());
    sphere->setType(rp3d::BodyType::DYNAMIC);

    auto* collider = sphere->addCollider(sphere_shape, rp3d::Transform::identity());
    collider->getMaterial().setBounciness(sphere_bounciness);

    _reset_sphere();
}


Scene::~Scene() noexcept {
    // The default material's shader belongs to raylib, so only the mesh is ours.
    UnloadMesh(sphere_mesh);

    // PhysicsCommon releases every world, shape and body it created.
}

auto Scene::_build_container() -> void {
    container = world->createRigidBody(rp3d::Transform::identity());
    container->setType(rp3d::BodyType::STATIC);

    for (const auto& face : container_faces()) {
        auto* shape = physics.createBoxShape(to_rp3d(face.half_extents));

        container->addCollider(
            shape,
            rp3d::Transform(to_rp3d(face.centre), rp3d::Quaternion::identity())
        );
    }
}


auto Scene::handle_events() -> void {
    // The drop is over in a few seconds and never repeats, so R replays it.
    if (IsKeyPressed(KEY_R)) {
        _reset_sphere();
    }

    cube.handle_events();
    player.handle_events();
}


auto Scene::_reset_sphere() -> void {
    sphere->setTransform(
        rp3d::Transform(spawn_position, rp3d::Quaternion::identity())
    );
    sphere->setLinearVelocity(spawn_velocity);
    sphere->setAngularVelocity(rp3d::Vector3::zero());
}


auto Scene::update(f64 delta) -> void {
    world->update(fixed_dt);

    cube.update(delta);
    player.update(delta);
}


auto Scene::render(f64 delta) -> void {
    _render_container();
    _render_sphere();
    cube.render(delta);
    player.render(delta);
}


[[nodiscard]] auto Scene::primary_camera() -> Camera3D { return player.get_camera(); }

[[nodiscard]] auto Scene::container_faces() const -> std::array<Box, num_faces> {
    const auto half_wall = wall_thickness / 2.0f;

    const auto span_x = interior.x + wall_thickness;
    const auto span_z = interior.z + wall_thickness;

    return {
        Box{.centre = {0.0f, -half_wall, 0.0f},
            .half_extents = {span_x, half_wall, span_z}},

        Box{.centre = {interior.x + half_wall, interior.y, 0.0f},
            .half_extents = {half_wall, interior.y, span_z}},

        Box{.centre = {-(interior.x + half_wall), interior.y, 0.0f},
            .half_extents = {half_wall, interior.y, span_z}},

        Box{.centre = {0.0f, interior.y, interior.z + half_wall},
            .half_extents = {span_x, interior.y, half_wall}},

        Box{.centre = {0.0f, interior.y, -(interior.z + half_wall)},
            .half_extents = {span_x, interior.y, half_wall}},
    };
}


// Floor solid, walls in wireframe, so the contents stay visible from outside.
auto Scene::_render_container() -> void {
    const auto faces = container_faces();

    DrawCubeV(faces[0].centre, full_extents(faces[0].half_extents), floor_colour);
    DrawCubeWiresV(faces[0].centre, full_extents(faces[0].half_extents), wall_colour);

    for (usize i = 1; i < faces.size(); ++i) {
        DrawCubeWiresV(
            faces[i].centre, full_extents(faces[i].half_extents), wall_colour
        );
    }
}

auto Scene::_render_sphere() -> void {
    const auto transform = to_raylib(sphere->getTransform());

    rlEnableWireMode();
    DrawMesh(sphere_mesh, sphere_material, transform);
    rlDisableWireMode();
}

} // namespace arcxel
