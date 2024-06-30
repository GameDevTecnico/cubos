#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/image/bridge.hpp>
#include <cubos/engine/image/plugin.hpp>

void cubos::engine::imagePlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);

    cubos.startupSystem("setup Image assets bridge").tagged(assetsBridgeTag).call([](Assets& assets) {
        stbi_set_flip_vertically_on_load(1);
        // Add the bridge to load .cubos files.
        auto bridge = std::make_shared<ImageBridge>();
        assets.registerBridge(".png", bridge);
        assets.registerBridge(".jpeg", bridge);
        assets.registerBridge(".jpg", bridge);
        assets.registerBridge(".bmp", bridge);
    });
}
