#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/plugin.hpp>

using cubos::core::ecs::Write;
using namespace cubos::engine;

static void bridge(Write<Assets> assets)
{
    // Add the bridge to load .cubos files.
    assets->registerBridge(".cubos", std::make_unique<SceneBridge>());
}

void cubos::engine::scenePlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
}
