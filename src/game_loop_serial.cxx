#include "game_loop.h"
#include "conf.h"
#include "timing.h"
#include "log.h"

#include <raylib.h>

namespace arcxel {

auto game_loop([[maybe_unused]] Config config, SampleRecord& global_sample_record) -> void {

	log(LogLevel::Info, "Starting [SERIAL] game loop");
    return serial_game_loop_fallback(config, global_sample_record);
}

} // namespace arcxel