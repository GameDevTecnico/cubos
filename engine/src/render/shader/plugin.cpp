#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/bridges/binary.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/shader/bridge.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;
using cubos::engine::ShaderBridge;

void cubos::engine::shaderPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.startupSystem("setup Shader asset bridge")
        .tagged(assetsBridgeTag)
        .after(windowInitTag)
        .call([](Assets& assets, Window& window) {
            // Add the bridge to load .glsl files.
            assets.registerBridge(".glsl", std::make_unique<ShaderBridge>(window->renderDevice()));
        });
}
