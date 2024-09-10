#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/shader/bridge.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::gl::Stage;
using cubos::core::io::Window;
using cubos::engine::ShaderBridge;

void cubos::engine::shaderPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(windowPlugin);

    cubos.startupSystem("setup Shader asset bridge")
        .tagged(assetsBridgeTag)
        .after(windowInitTag)
        .call([](Assets& assets, Window& window) {
            // Add bridges for vertex and pixel shaders.
            assets.registerBridge(".vs", std::make_unique<ShaderBridge>(window->renderDevice(), Stage::Vertex));
            assets.registerBridge(".gs", std::make_unique<ShaderBridge>(window->renderDevice(), Stage::Geometry));
            assets.registerBridge(".fs", std::make_unique<ShaderBridge>(window->renderDevice(), Stage::Pixel));
        });
}
