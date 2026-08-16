#include "player.h"

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

namespace arcxel {

Player::Player() noexcept
    : speed(5.0f)
    , sprint_speed_scale(1.75f)
    , look_sensitivity(0.0015f) {
    camera.position = Vector3Zero();
    camera.target = Vector3{0.0f, 0.0f, -1.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0;
    camera.projection = CameraProjection::CAMERA_PERSPECTIVE;
}

[[nodiscard]] auto Player::get_camera() -> Camera3D { return camera; }

auto Player::handle_events() -> void {}

auto Player::update(f64 delta) -> void {
    _look_controls(delta);
    _movement_controls(delta);
}

auto Player::render(f64 delta) -> void {}

auto Player::_movement_controls(f64 delta) -> void {
    auto speed_delta = speed * static_cast<f32>(delta);

    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        speed_delta *= sprint_speed_scale;
    }

    if (IsKeyDown(KEY_W)) {
        CameraMoveForward(&camera, speed_delta, false);
    }

    if (IsKeyDown(KEY_A)) {
        CameraMoveRight(&camera, -speed_delta, false);
    }

    if (IsKeyDown(KEY_S)) {
        CameraMoveForward(&camera, -speed_delta, false);
    }

    if (IsKeyDown(KEY_D)) {
        CameraMoveRight(&camera, speed_delta, false);
    }

    if (IsKeyDown(KEY_Q)) {
        CameraMoveUp(&camera, -speed_delta);
    }

    if (IsKeyDown(KEY_E)) {
        CameraMoveUp(&camera, speed_delta);
    }
}

auto Player::_look_controls(f64 delta) -> void {
    auto mouse = GetMouseDelta();
    CameraYaw(&camera, -mouse.x * look_sensitivity, false);
    CameraPitch(&camera, -mouse.y * look_sensitivity, true, false, false);
}

} // namespace arcxel
