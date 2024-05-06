#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::resizeRenderTargetTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToRenderTargetTag);

void cubos::engine::renderTargetPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);

    cubos.component<RenderTarget>();

    cubos.tag(resizeRenderTargetTag);
    cubos.tag(drawToRenderTargetTag).after(resizeRenderTargetTag).before(windowRenderTag);

    cubos.system("resize render targets")
        .tagged(resizeRenderTargetTag)
        .call([](const Window& window, Query<RenderTarget&> query) {
            for (auto [target] : query)
            {
                if (target.framebuffer == nullptr && window->framebufferSize() != target.size)
                {
                    target.size = window->framebufferSize();

                    CUBOS_INFO("Resized RenderTarget to {}x{}", target.size.x, target.size.y);
                }

                target.cleared = false;
            }
        });
}
