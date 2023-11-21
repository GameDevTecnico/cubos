#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/plugin.hpp>

using cubos::core::ecs::World;

using namespace cubos::engine;

static void bridge(Write<World> world)
{
    auto assets = world->write<Assets>();

    // Add the bridge to load .cubos files.
    assets.get().registerBridge(".cubos", std::make_unique<SceneBridge>(world->components()));
}

void cubos::engine::scenePlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
}
