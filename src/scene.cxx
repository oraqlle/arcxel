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

// rp3d takes half-extents, raylib draws with full ones. Converting in a single
// place is what stops the two descriptions of the container disagreeing.
[[nodiscard]] constexpr auto full_extents(Vector3 half) -> Vector3 {
    return Vector3{half.x * 2.0f, half.y * 2.0f, half.z * 2.0f};
}

} // namespace


Scene::Scene() noexcept
    : interior(default_interior)
    , wall_thickness(default_wall_thickness)
    , sphere_radius(default_sphere_radius)
    , spin(0.0f) {
    // Sitting on the floor, as a reference for scale until the sphere replaces it.
    cube.transform.position = Vector3{0.0f, 0.5f, -5.0f};

    // Needs a live GL context, so Scene must outlive nothing and the window must
    // outlive it. main.cxx opens the window before Engine is constructed.
    sphere_mesh = GenMeshSphere(sphere_radius, sphere_rings, sphere_slices);
    sphere_material = LoadMaterialDefault();
    sphere_material.maps[MATERIAL_MAP_DIFFUSE].color = sphere_colour;
}


Scene::~Scene() noexcept {
    // The default material's shader belongs to raylib, so only the mesh is ours.
    UnloadMesh(sphere_mesh);
}


auto Scene::handle_events() -> void {
    cube.handle_events();
    player.handle_events();
}


auto Scene::update(f64 delta) -> void {
    cube.update(delta);
    player.update(delta);
}


auto Scene::render(f64 delta) -> void {
    _render_container();
    _render_sphere(delta);
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

auto Scene::_render_sphere(f64 delta) -> void {
    spin += static_cast<f32>(delta);

    const auto rotation = MatrixRotateXYZ(Vector3{spin * 0.7f, spin, spin * 0.3f});
    const auto translation = MatrixTranslate(0.0f, 3.0f, 0.0f);

    // Rotate, then translate; the same order DrawModelEx uses internally.
    const auto transform = MatrixMultiply(rotation, translation);

    rlEnableWireMode();
    DrawMesh(sphere_mesh, sphere_material, transform);
    rlDisableWireMode();
}

} // namespace arcxel
