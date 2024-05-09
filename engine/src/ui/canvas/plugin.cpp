#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/ui/canvas/horizontal_stretch.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/canvas/ui_canvas.hpp>
#include <cubos/engine/ui/canvas/ui_element.hpp>
#include <cubos/engine/ui/canvas/vertical_stretch.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::elementVPUpdateTag);
CUBOS_DEFINE_TAG(cubos::engine::elementPropagateTag)
CUBOS_DEFINE_TAG(cubos::engine::uiBeginTag)
CUBOS_DEFINE_TAG(cubos::engine::uiDrawTag)

using namespace cubos::engine;

void cubos::engine::canvasPlugin(Cubos& cubos)
{
    cubos.component<UIElement>();
    cubos.component<UICanvas>();
    cubos.component<HorizontalStretch>();
    cubos.component<VerticalStretch>();

    cubos.depends(windowPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(transformPlugin);

    cubos.tag(elementVPUpdateTag);
    cubos.tag(elementPropagateTag).after(elementVPUpdateTag);
    cubos.tag(uiBeginTag).after(elementPropagateTag);
    cubos.tag(uiDrawTag).after(uiBeginTag).before(windowRenderTag);

    cubos.observer("initialize Canvas").onAdd<UICanvas>().call([](Query<UICanvas&> query) {
        for (auto [canvas] : query)
        {
            canvas.mat = glm::ortho<float>(0, canvas.width, 0, canvas.height, -10000, 100000);
        }
    });
    cubos.system("set element matrix")
        .tagged(elementVPUpdateTag)
        .with<UIElement>()
        .withOpt<HorizontalStretch>()
        .withOpt<VerticalStretch>()
        .related<ChildOf>()
        .with<UICanvas>()
        .call([](Query<UIElement&, Opt<const HorizontalStretch&>, Opt<const VerticalStretch&>, const UICanvas&> query) {
            for (auto [element, hs, vs, canvas] : query)
            {
                element.vp = canvas.mat;
                element.position = element.anchor * glm::vec2{canvas.width, canvas.height} + element.offset;
                if (hs.contains())
                {
                    element.size.x = canvas.width - hs.value().rightOffset - hs.value().leftOffset;
                    element.position.x = hs.value().leftOffset + element.size.x * element.pivot.x + element.offset.x;
                }
                if (vs.contains())
                {
                    element.size.y = canvas.height - vs.value().topOffset - vs.value().bottomOffset;
                    element.position.y = vs.value().bottomOffset + element.size.y * element.pivot.y + element.offset.y;
                }
            }
        });

    cubos.system("set child position")
        .tagged(elementPropagateTag)
        .with<UIElement>()
        .withOpt<HorizontalStretch>()
        .withOpt<VerticalStretch>()
        .related<ChildOf>(core::ecs::Traversal::Down)
        .with<UIElement>()
        .call([](Query<UIElement&, Opt<const HorizontalStretch&>, Opt<const VerticalStretch&>, const UIElement&>
                     children) {
            for (auto [child, hs, vs, parent] : children)
            {
                child.position = child.anchor * parent.topRightCorner() +
                                 (glm::vec2{1, 1} - child.anchor) * parent.bottomLeftCorner() + child.offset;
                child.vp = parent.vp;
                child.hierarchyDepth = parent.hierarchyDepth + 1;
                if (hs.contains())
                {
                    child.size.x = parent.topRightCorner().x - hs.value().rightOffset - parent.bottomLeftCorner().x -
                                   hs.value().leftOffset;
                    child.position.x = parent.bottomLeftCorner().x + hs.value().leftOffset +
                                       child.size.x * child.pivot.x + child.offset.x;
                }
                if (vs.contains())
                {
                    child.size.y = parent.topRightCorner().y - vs.value().topOffset - parent.bottomLeftCorner().y -
                                   vs.value().bottomOffset;
                    child.position.y = parent.bottomLeftCorner().y + vs.value().bottomOffset +
                                       child.size.y * child.pivot.y + child.offset.y;
                }
            }
        });

    cubos.system("begin UI frame")
        .tagged(uiBeginTag)
        .call([](const core::io::Window& window, Query<UICanvas&, RenderTarget&> targets) {
            for (auto [canvas, target] : targets)
            {
                window->renderDevice().setFramebuffer(target.framebuffer);
                window->renderDevice().setScissor(0, 0, static_cast<int>(target.size.x),
                                                  static_cast<int>(target.size.y));
                if (!target.cleared)
                {
                    window->renderDevice().clearColor(0.0F, 0.0F, 0.0F, 0.0F);
                    target.cleared = true;
                }
                window->renderDevice().clearDepth(1);
            }
        });
}