#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/plugin.hpp>

using cubos::core::ecs::World;
using cubos::core::reflection::TypeRegistry;

void cubos::engine::scenePlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);

    cubos.startupSystem("setup Scene assets bridge").tagged(assetsBridgeTag).call([](World& world, Assets& assets) {
        // Add the bridge to load .cubos files.
        assets.registerBridge(".cubos",
                              std::make_unique<SceneBridge>(world.types().components(), world.types().relations()));
    });
}
