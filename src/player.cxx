#include "player.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

namespace arcxel {

    [[nodiscard]] static auto axis_vec3_to_mat4(
        const Vector3& forward,
        const Vector3& right,
        const Vector3& up) -> Matrix {
		return Matrix{
            right.x,     right.y,     right.z,     0.0f,
            up.x,        up.y,        up.z,        0.0f,
            -forward.x,  -forward.y,  -forward.z,  0.0f,
            0.0f,        0.0f,        0.0f,        1.0f
        };
	}


Player::Player() noexcept
    : GameObject(TransformIdentity)
    , speed(10.0f)
    , sprint_speed_scale(3.75f)
    , look_sensitivity(0.0015f) {
    camera.position = Vector3{ 100.0f, 80.0f, 0.0f };
    camera.target = Vector3{ 0.0f, 0.0f, -1.0f };
    camera.up = Vector3UnitY;
    camera.fovy = 45.0;
    camera.projection = CameraProjection::CAMERA_PERSPECTIVE;
}


[[nodiscard]] auto Player::get_camera() -> Camera3D { return camera; }


auto Player::update(f64 delta) -> void {
    _look_controls(delta);
    _movement_controls(delta);

    transform.translation = camera.position; //< Update transform for consistency

    const auto forward = Vector3Normalize(camera.target - camera.position);
    const auto right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    const auto up = Vector3CrossProduct(right, forward);
    transform.rotation = QuaternionFromMatrix(axis_vec3_to_mat4(forward, right, up));
}


auto Player::render(f64) -> void {}


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


auto Player::_look_controls(f64) -> void {
    auto mouse = GetMouseDelta();
    CameraYaw(&camera, -mouse.x * look_sensitivity, false);
    CameraPitch(&camera, -mouse.y * look_sensitivity, true, false, false);
}

} // namespace arcxel
