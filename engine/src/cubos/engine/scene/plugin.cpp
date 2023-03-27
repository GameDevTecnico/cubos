#include <cubos/engine/scene/plugin.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/bridge.hpp>

using namespace cubos::engine;

static void bridge(Assets& assets)
{
    // Add the bridge to load .scn files.
    assets.registerBridge(".scn", std::make_unique<SceneBridge>());
}

void cubos::engine::scenePlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
}
