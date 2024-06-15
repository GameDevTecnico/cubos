#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/hdr/hdr.hpp>
#include <cubos/engine/render/hdr/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::gl::Texture2DDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::createHDRTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToHDRTag);

void cubos::engine::hdrPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);
    cubos.depends(renderTargetPlugin);

    cubos.component<HDR>();

    cubos.tag(createHDRTag).after(resizeRenderTargetTag);
    cubos.tag(drawToHDRTag).after(createHDRTag);

    cubos.system("resize HDR textures")
        .tagged(createHDRTag)
        .call([](const Window& window, Query<const RenderTarget&, HDR&> query) {
            for (auto [target, hdr] : query)
            {
                if (target.size != hdr.size)
                {
                    hdr.size = target.size;

                    // Recreate both textures.
                    Texture2DDesc desc{
                        .width = hdr.size.x,
                        .height = hdr.size.y,
                        .usage = Usage::Dynamic,
                        .format = TextureFormat::RGBA32Float,
                    };
                    hdr.frontTexture = window->renderDevice().createTexture2D(desc);
                    hdr.backTexture = window->renderDevice().createTexture2D(desc);

                    CUBOS_INFO("Resized HDR to {}x{}", hdr.size.x, hdr.size.y);
                }
            }
        });
}
