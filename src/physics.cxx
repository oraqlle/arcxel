#include "physics.h"
#include "rp3d.h"

namespace arcxel {

Physics::Physics(std::optional<rp3d::PhysicsWorld::WorldSettings> opt_settings) noexcept
    : common{} {
    if (opt_settings) {
        world = common.createPhysicsWorld(*opt_settings);
    } else {
        world = common.createPhysicsWorld();
    }
}


Physics::~Physics() noexcept = default;


[[nodiscard]] auto
Physics::singleton(std::optional<rp3d::PhysicsWorld::WorldSettings> init) -> Physics& {
    static auto physics_sys = Physics(std::move(init));
    return physics_sys;
}


auto Physics::update(f64 delta) -> void {
    accumulator += delta;

    // While there is enough accumulated time to take
    // one or several physics steps
    while (accumulator >= timestep) {
        world->update(timestep);
        accumulator -= timestep;
    }
}

} // namespace arcxel
