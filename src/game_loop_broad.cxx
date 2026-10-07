#include "game_loop.h"
#include "conf.h"
#include "single_item_channel.h"
#include "game_object.h"
#include "engine.h"
#include "physics.h"
#include "timing.h"
#include "types.h"
#include "log.h"

#include <raylib.h>

#include <stop_token>
#include <thread>
#include <ranges>
#include <vector>

namespace arcxel {

using Label = Sample::Label;
using PacketChannel = SingleItemChannel<FramePacket>;
using ArrayChannel = SingleItemChannel<ObjectsArray>;


static auto update_packet(FramePacket packet) -> void {
    for (auto& obj : packet.objects) {
        obj->update(packet.delta);
    }
}


static auto physics_sim_worker(
    std::stop_token stop_tkn,
    PacketChannel& src,
    PacketChannel& sink,
    SampleRecord& samples
) -> void {
    auto packet = FramePacket{};

    while (!stop_tkn.stop_requested() && src.pop(packet)) {
        {
            const auto _ = Timespan(Label::PhysicsUpdate, samples);
            update_packet(packet);
        }

        sink.push(std::move(packet));
    }

    sink.close();
}


static auto update_sim_worker(
    std::stop_token stop_tkn,
    PacketChannel& src,
    PacketChannel& sink,
    SampleRecord& samples
) -> void {
    auto packet = FramePacket{};

    while (!stop_tkn.stop_requested() && src.pop(packet)) {
        {
            const auto _ = Timespan(Label::Update, samples);
            Physics::singleton().update(packet.delta);
        }

        sink.push(std::move(packet));
    }

    sink.close();
}


auto game_loop([[maybe_unused]] Config config, SampleRecord& global_sample_record) -> void {

    if (config.num_available_threads < config.num_required_threads) {
        log(LogLevel::Info, "Starting [SERIAL] game loop [FALLBACK OF BROAD]");
        return serial_game_loop_fallback(config, global_sample_record);
    }

    log(LogLevel::Info, "Starting [BROAD] game loop");

    constexpr usize threaded_max_num_samples = SampleRecord::MAX_SAMPLES / 4;

    auto frame_and_render_samples = SampleRecord(threaded_max_num_samples * 2);
    auto physics_samples = SampleRecord(threaded_max_num_samples);
    auto update_samples = SampleRecord(threaded_max_num_samples);

    auto physics_channel = PacketChannel{};
    auto update_channel = PacketChannel{};
    auto render_channel = PacketChannel{};


    // ---- PhysicsUpdate Thread ----
    auto physics_thread = std::jthread(
        physics_sim_worker,
        std::ref(physics_channel),
        std::ref(update_channel),
        std::ref(physics_samples)
    );


    // ---- Update Thread ----
    //auto update_thread = std::jthread(
    //    physics_sim_worker,
    //    update_channel,
    //    render_channel,
    //    update_samples
    //);

    auto render_packet = FramePacket{};

    // ---- Render Thread [Main] ----
    while (Engine::singleton().is_running()) {
        const auto frame_span = arcxel::Timespan(Label::Frame, global_sample_record);
        const f64 delta = GetFrameTime();

        physics_channel.push(FramePacket{
            .objects = std::move(Engine::singleton().get_scene().objects), //< feed objects to physics
            .delta = delta
        });

        /**
         * Render the oldest completed frame. This blocks only when the
         * pipeline has not produced a frame yet.
         */
        if (!render_channel.pop(render_packet)) {
            break;
        }

        Engine::singleton().get_scene().objects = std::move(render_packet.objects);

        {
            const auto _ = Timespan(Label::Render, global_sample_record);
            Engine::singleton().render(render_packet.delta, global_sample_record);
        }
    }

    physics_channel.close();

    physics_thread.request_stop();
    //update_thread.request_stop();

    physics_thread.join();
    //update_thread.join();

    global_sample_record += frame_and_render_samples;
    global_sample_record += physics_samples;
    global_sample_record += update_samples;
}

} // namespace arcxel