#include "physics_object.h"
#include "conf.h"
#include "physics.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>

namespace arcxel {

PhysicsObject::PhysicsObject(
    rp3d::BodyType btype,
    Transform transform,
    Color colour) noexcept
    : GameObject(transform)
    , colour(colour)
    , mesh({})
    , model({})
    , body(nullptr) {

    const auto phys_pos = as(transform.translation);
    const auto phys_rot = as(transform.rotation);
    auto phys_transform = rp3d::Transform{ phys_pos, phys_rot };
    body = Physics::singleton().world->createRigidBody(phys_transform);
    body->setType(btype);

    if constexpr (physics_debug_renderer_enabled) {
        body->setIsDebugEnabled(true);
    }
}

PhysicsObject::~PhysicsObject() noexcept {
    if (body) {
        Physics::singleton().world->destroyRigidBody(body);
        body = nullptr;
    }

    if (IsModelValid(model)) {
        UnloadModel(model);

        model = {};
        mesh = {};
    }
}


auto PhysicsObject::handle_events() -> void {};


auto PhysicsObject::update(f64) -> void { _M_sync_model_to_physics(); }


auto PhysicsObject::render(f64) -> void {
    auto axis = Vector3{};
    auto angle = f32{};
    QuaternionToAxisAngle(QuaternionNormalize(transform.rotation), &axis, &angle);
    DrawModelEx(
        model,
        transform.translation,
        axis,
        angle * RAD2DEG,
        transform.scale,
        colour);

    if constexpr (physics_debug_renderer_enabled) {
        _M_render_physics_debug_shapes();
    }
}


auto PhysicsObject::_M_sync_model_to_physics() -> void {
    transform = as(body->getTransform());
}


auto PhysicsObject::_M_render_physics_debug_shapes() -> void {
    if (body->isDebugEnabled()) {
        auto& world = Physics::singleton().world;
        auto& dbgr = world->getDebugRenderer();

        const auto num_lines = dbgr.getNbLines();
        const auto& lines = dbgr.getLines();

        const auto num_triangles = dbgr.getNbTriangles();
        const auto& triangles = dbgr.getTriangles();

        for (auto idx = u32{ 0 }; idx < num_lines; ++idx) {

            const auto& line = lines[idx];
            const auto start = as(line.point1);
            const auto end = as(line.point2);

            const auto r = static_cast<u8>((line.color1 >> 24) & 0xff);
            const auto g = static_cast<u8>((line.color1 >> 16) & 0xff);
            const auto b = static_cast<u8>((line.color1 >> 8) & 0xff);
            const auto colour = Color{ r, g, b, 255 };

            DrawLine3D(start, end, colour);
        }

        for (auto idx = u32{ 0 }; idx < num_triangles; ++idx) {

            const auto& triangle = triangles[idx];
            const auto p1 = as(triangle.point1);
            const auto p2 = as(triangle.point2);
            const auto p3 = as(triangle.point3);

            const auto r = static_cast<u8>((triangle.color1 >> 24) & 0xff);
            const auto g = static_cast<u8>((triangle.color1 >> 16) & 0xff);
            const auto b = static_cast<u8>((triangle.color1 >> 8) & 0xff);
            const auto colour = Color{ r, g, b, 255 };

            DrawLine3D(p1, p2, colour);
            DrawLine3D(p2, p3, colour);
            DrawLine3D(p3, p1, colour);
        }
    }
}

} // namespace arcxel
