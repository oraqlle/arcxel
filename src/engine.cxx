#include "engine.h"
#include "timing.h"

#include <raylib.h>

namespace arcxel {

Engine::Engine()
    : sample_record()
    , running(true)
    , scene(std::nullopt) {}


[[nodiscard]] auto Engine::singleton() -> Engine& {

    static auto engine = Engine();
    return engine;
}


[[nodiscard]] auto Engine::is_running() -> bool {
    return running && !WindowShouldClose();
}


// expand to drop the scene so objects freed in while logger is open
auto Engine::stop() -> void {
    running = false;

    if (scene) {
		scene.value().unload();
    }
}



auto Engine::load_scene(Scene&& scene) -> void {
    this->scene.emplace(std::move(scene));
}


auto Engine::unload_scene() -> std::optional<Scene> {
    if (scene) {
        auto&& _scene = std::move(scene);
		scene.reset();
		return std::move(_scene);
    }
    
    return std::nullopt;
}


[[nodiscard]] auto Engine::get_scene() -> std::optional<Scene>& {
    return scene;
}


auto Engine::handle_events() -> void {
    if (scene) {
        scene.value().handle_events();
    }
}


auto Engine::update(f64 delta) -> void {
    if (scene) {
        scene.value().update(delta);
    }
}


auto Engine::render(f64 delta) -> void {
    if (scene) {
        auto& _scene = *scene;
		auto camera = _scene.primary_camera();

		{
			const auto span = Timespan(Sample::Label::Construct, sample_record);
			BeginDrawing();
			ClearBackground(RAYWHITE);
		}

		{
			const auto span = Timespan(Sample::Label::Draw, sample_record);
			BeginMode3D(camera);
			_scene.render(delta);
			EndMode3D();
		}

		{
			const auto span = Timespan(Sample::Label::Present, sample_record);
			EndDrawing();
		}

    }
}


} // namespace arcxel
