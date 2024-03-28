#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/depth/depth.hpp>
#include <cubos/engine/render/depth/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::gl::Texture2DDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::createRenderDepthTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToRenderDepthTag);

void cubos::engine::renderDepthPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);
    cubos.depends(renderTargetPlugin);

    cubos.component<RenderDepth>();

    cubos.tag(createRenderDepthTag);
    cubos.tag(drawToRenderDepthTag).after(createRenderDepthTag);

    cubos.system("resize RenderDepths")
        .tagged(createRenderDepthTag)
        .after(resizeRenderTargetTag)
        .call([](const Window& window, Query<const RenderTarget&, RenderDepth&> query) {
            for (auto [target, depth] : query)
            {
                if (target.size != depth.size)
                {
                    depth.size = target.size;

                    // Recreate depth texture.
                    depth.texture = window->renderDevice().createTexture2D({
                        .width = depth.size.x,
                        .height = depth.size.y,
                        .usage = Usage::Dynamic,
                        .format = TextureFormat::Depth32,
                    });

                    CUBOS_INFO("Resized RenderDepth to {}x{}", depth.size.x, depth.size.y);
                }
            }
        });
}