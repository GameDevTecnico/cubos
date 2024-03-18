#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/g_buffer/g_buffer.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>

using cubos::core::gl::Texture2DDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::resizeRenderTargetTag);
CUBOS_DEFINE_TAG(cubos::engine::createGBufferTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToGBufferTag);

void cubos::engine::gBufferPlugin(Cubos& cubos)
{
    cubos.component<GBuffer>();

    cubos.tag(createGBufferTag).after(resizeRenderTargetTag);
    cubos.tag(drawToGBufferTag).after(createGBufferTag);

    cubos.system("resize GBuffers")
        .tagged(createGBufferTag)
        .call([](const Window& window, Query<const RenderTarget&, GBuffer&> query) {
            for (auto [target, gBuffer] : query)
            {
                if (target.size != gBuffer.size)
                {
                    gBuffer.size = target.size;

                    // Prepare common texture description.
                    Texture2DDesc desc{};
                    desc.width = gBuffer.size.x;
                    desc.height = gBuffer.size.y;
                    desc.usage = Usage::Dynamic;

                    auto& rd = window->renderDevice();

                    // Create position and normal textures.
                    desc.format = TextureFormat::RGB32Float;
                    gBuffer.position = rd.createTexture2D(desc);
                    gBuffer.normal = rd.createTexture2D(desc);

                    // Create albedo texture.
                    desc.format = TextureFormat::RGBA8UNorm;
                    gBuffer.albedo = rd.createTexture2D(desc);

                    CUBOS_INFO("Resized GBuffer to {}x{}", gBuffer.size.x, gBuffer.size.y);
                }
            }
        });
}
