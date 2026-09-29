#include "utils.h"
#include "log.h"

#include <chrono>

namespace arcxel {

[[nodiscard]] auto create_dir(const std::string_view dirname)
    -> std::expected<std::filesystem::path, std::string> {

    const auto path = std::filesystem::weakly_canonical(dirname);

    // Check if filesystem object of the same name exists
    if (!std::filesystem::exists(path)) {
        log(LogLevel::Info, "Creating logs directory at path '{}'", path.string());
        std::filesystem::create_directories(path);
    } else if (
        std::filesystem::status(path).type() != std::filesystem::file_type::directory) {
        return std::unexpected(
            std::format("Path '{}' exists but is not a directory", path.string()));
    }

    return { path };
}


[[nodiscard]] auto current_datetime()
    -> std::chrono::local_time<std::chrono::system_clock::duration> {
    return std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
}



[[nodiscard]] auto as(const rp3d::Vector3& vec) -> Vector3 {
    return Vector3{ vec.x, vec.y, vec.z };
}


[[nodiscard]] auto as(const Vector3& vec) -> rp3d::Vector3 {
    return rp3d::Vector3{ vec.x, vec.y, vec.z };
}


[[nodiscard]] auto as(const rp3d::Quaternion& quat) -> Quaternion {
    return Quaternion{ quat.x, quat.y, quat.z, quat.w };
}


[[nodiscard]] auto as(const Quaternion& quat) -> rp3d::Quaternion {
    return rp3d::Quaternion{ quat.x, quat.y, quat.z, quat.w };
}


[[nodiscard]] auto as(const rp3d::Transform& transform) -> Transform {
    return Transform{ .translation = as(transform.getPosition()),
                      .rotation = as(transform.getOrientation()),
                      .scale = Vector3{ 1.0f, 1.0f, 1.0f } };
}


[[nodiscard]] auto as(const Transform& transform) -> rp3d::Transform {
    return rp3d::Transform(as(transform.translation), as(transform.rotation));
}

} // namespace arcxel
