#include <memory>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/font/atlas/bridge.hpp>
#include <cubos/engine/font/bridge.hpp>
#include <cubos/engine/font/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

void cubos::engine::fontPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(windowPlugin);

    cubos.startupSystem("setup Font assets bridge").tagged(assetsBridgeTag).call([](Assets& assets) {
        auto bridge = std::make_shared<FontBridge>();
        assets.registerBridge(".ttf", bridge);
        assets.registerBridge(".otf", bridge);
    });

    cubos.startupSystem("setup FontAtlas assets bridge")
        .tagged(assetsBridgeTag)
        .call([](Assets& assets, const core::io::Window& window) {
            auto bridge = std::make_shared<FontAtlasBridge>(window->renderDevice());
            assets.registerBridge(".fontatlas", bridge);
        });
}
