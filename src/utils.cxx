#include "utils.h"
#include "log.h"

#include <chrono>
#include <ctime>

namespace arcxel {

[[nodiscard]] auto create_dir(const std::string_view dirname)
    -> std::expected<std::filesystem::path, std::string> {

    const auto path = std::filesystem::weakly_canonical(dirname);

    // Check if filesystem object of the same name exists
    if (!std::filesystem::exists(path)) {
        log(LogLevel::Info, "Creating logs directory at path '{}'", path.string());
        std::filesystem::create_directories(path);
    } else if (std::filesystem::status(path).type() !=
               std::filesystem::file_type::directory) {
        return std::unexpected(
            std::format("Path '{}' exists but is not a directory", path.string())
        );
    }

    return {path};
}


[[nodiscard]] auto current_datetime()
    -> std::chrono::local_time<std::chrono::system_clock::duration> {
    const auto now = std::chrono::system_clock::now();

#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L
    return std::chrono::current_zone()->to_local(now);
#else
    // libc++ ships no tzdb, so the UTC offset comes from the C library instead.
    const auto secs = std::chrono::system_clock::to_time_t(now);

    auto parts = std::tm{};
    localtime_r(&secs, &parts);

    return std::chrono::local_time<std::chrono::system_clock::duration>(
        now.time_since_epoch() + std::chrono::seconds(parts.tm_gmtoff)
    );
#endif
}

} // namespace arcxel
