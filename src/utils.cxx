#include "utils.h"
#include "log.h"

namespace arcxel {

[[nodiscard]] auto create_dir(const std::string_view dirname)
    -> std::expected<std::filesystem::path, std::string> {

    const auto path = std::filesystem::weakly_canonical(dirname);

    // Check if filesystem object of the same name exists
    if (!std::filesystem::exists(path)) {
        log::log(
            log::LogLevel::Info, "Creating logs directory at path '{}'", path.string()
        );
        std::filesystem::create_directories(path);
    } else if (std::filesystem::status(path).type() !=
               std::filesystem::file_type::directory) {
        return std::unexpected(
            std::format("Path '{}' exists but is not a directory", path.string())
        );
    }

    return {path};
}

} // namespace arcxel
