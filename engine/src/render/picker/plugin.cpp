#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/picker/picker.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::gl::Texture2DDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::createRenderPickerTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToRenderPickerTag);

void cubos::engine::renderPickerPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);
    cubos.depends(renderTargetPlugin);

    cubos.component<RenderPicker>();

    cubos.tag(createRenderPickerTag).after(resizeRenderTargetTag);
    cubos.tag(drawToRenderPickerTag).after(createRenderPickerTag);

    cubos.system("resize RenderPickers")
        .tagged(createRenderPickerTag)
        .call([](const Window& window, Query<const RenderTarget&, RenderPicker&> query) {
            for (auto [target, renderPicker] : query)
            {
                if (target.size != renderPicker.size)
                {
                    renderPicker.size = target.size;

                    // Prepare common texture description.
                    Texture2DDesc desc{};
                    desc.width = renderPicker.size.x;
                    desc.height = renderPicker.size.y;
                    desc.usage = Usage::Dynamic;
                    desc.format = TextureFormat::RG16UInt;

                    auto& rd = window->renderDevice();

                    renderPicker.backTexture = rd.createTexture2D(desc);
                    renderPicker.frontTexture = rd.createTexture2D(desc);

                    // Since copyTo outputs only RGBA data with 4 bytes per channel, we need to allocate 16 bytes per
                    // pixel.
                    renderPicker.pixelBuffer = rd.createPixelPackBuffer(desc.width * desc.height * 4 * 4);

                    CUBOS_INFO("Resized RenderPicker to {}x{}", renderPicker.size.x, renderPicker.size.y);
                }

                // Swap textures
                std::swap(renderPicker.backTexture, renderPicker.frontTexture);

                // Copy the back texture to the pixel buffer.
                if (renderPicker.backTexture != nullptr)
                {
                    renderPicker.backTexture->copyTo(0, 0, renderPicker.size.x, renderPicker.size.y,
                                                     renderPicker.pixelBuffer);
                }

                // New frame, hint that the front texture needs to be cleared.
                renderPicker.cleared = false;
            }
        });
}
