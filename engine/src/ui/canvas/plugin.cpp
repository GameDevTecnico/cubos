#include <map>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/ui/canvas/canvas.hpp>
#include <cubos/engine/ui/canvas/draw_list.hpp>
#include <cubos/engine/ui/canvas/element.hpp>
#include <cubos/engine/ui/canvas/expand.hpp>
#include <cubos/engine/ui/canvas/horizontal_stretch.hpp>
#include <cubos/engine/ui/canvas/keep_pixel_size.hpp>
#include <cubos/engine/ui/canvas/match_height.hpp>
#include <cubos/engine/ui/canvas/match_width.hpp>
#include <cubos/engine/ui/canvas/native_aspect_ratio.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/canvas/vertical_stretch.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::uiCanvasChildrenUpdateTag);
CUBOS_DEFINE_TAG(cubos::engine::uiElementPropagateTag)
CUBOS_DEFINE_TAG(cubos::engine::uiBeginTag)
CUBOS_DEFINE_TAG(cubos::engine::uiDrawTag)
CUBOS_DEFINE_TAG(cubos::engine::uiEndTag)

using namespace cubos::engine;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        cubos::core::gl::DepthStencilState dss;
        cubos::core::gl::ConstantBuffer mvpBuffer;
        State(cubos::core::gl::DepthStencilState depthStencilState, cubos::core::gl::ConstantBuffer mvpConstantBuffer)
            : dss(std::move(depthStencilState))
            , mvpBuffer(std::move(mvpConstantBuffer))
        {
        }
    };
} // namespace

static void copyCommands(Query<Entity, UIElement&, UIElement&> query, UIElement& element, Entity entity,
                         std::map<const UIDrawList::Type*, UIDrawList>& commands)
{
    for (size_t i = 0; i < element.drawList.size(); i++)
    {
        auto entry = element.drawList.entry(i);
        commands[&entry.type].push(entry.type, entry.command, element.drawList.data(i));
    }
    element.drawList.clear();
    for (auto [child, childElement, _] : query.pin(1, entity))
    {
        copyCommands(query, childElement, child, commands);
    }
}

void cubos::engine::uiCanvasPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(transformPlugin);

    cubos.component<UIElement>();
    cubos.component<UICanvas>();

    // TODO: Make these exclusive
    cubos.component<UIKeepPixelSize>();
    cubos.component<UIMatchHeight>();
    cubos.component<UIMatchWidth>();
    cubos.component<UIExpand>();

    cubos.component<UIHorizontalStretch>();
    cubos.component<UIVerticalStretch>();
    cubos.component<UINativeAspectRatio>();

    cubos.uninitResource<State>();

    cubos.tag(uiCanvasChildrenUpdateTag);
    cubos.tag(uiElementPropagateTag).after(uiCanvasChildrenUpdateTag);
    cubos.tag(uiBeginTag).after(uiElementPropagateTag);
    cubos.tag(uiDrawTag).after(uiBeginTag);
    cubos.tag(uiEndTag).after(uiDrawTag).tagged(drawToRenderTargetTag);

    cubos.startupSystem("setup canvas state")
        .after(windowInitTag)
        .call([](Commands cmds, const core::io::Window& window) {
            cubos::core::gl::DepthStencilStateDesc dssd;
            dssd.depth.enabled = true;
            dssd.depth.writeEnabled = true;
            dssd.depth.near = -1.0F;
            dssd.depth.compare = Compare::LEqual;
            cmds.emplaceResource<State>(window->renderDevice().createDepthStencilState(dssd),
                                        window->renderDevice().createConstantBuffer(sizeof(glm::mat4), nullptr,
                                                                                    cubos::core::gl::Usage::Dynamic));
        });

    cubos.system("scale canvas")
        .call([](Query<UICanvas&, const RenderTarget&, Opt<const UIKeepPixelSize&>, Opt<const UIMatchHeight&>,
                       Opt<const UIMatchWidth&>, Opt<const UIExpand&>>
                     query) {
            for (auto [canvas, rt, kpis, mh, mw, exp] : query)
            {
                if (kpis.contains())
                {
                    canvas.virtualSize = rt.size;
                }
                else if (mh.contains())
                {
                    canvas.virtualSize.y = canvas.referenceSize.y;
                    canvas.virtualSize.x = canvas.virtualSize.y * (float)rt.size.x / (float)rt.size.y;
                }
                else if (mw.contains())
                {
                    canvas.virtualSize.x = canvas.referenceSize.x;
                    canvas.virtualSize.y = canvas.virtualSize.x * (float)rt.size.y / (float)rt.size.x;
                }
                else if (exp.contains())
                {
                    if ((float)rt.size.y / (float)rt.size.x > canvas.referenceSize.y / canvas.referenceSize.x)
                    {
                        canvas.virtualSize.x = canvas.referenceSize.x;
                        canvas.virtualSize.y = canvas.virtualSize.x * (float)rt.size.y / (float)rt.size.x;
                    }
                    else
                    {
                        canvas.virtualSize.y = canvas.referenceSize.y;
                        canvas.virtualSize.x = canvas.virtualSize.y * (float)rt.size.x / (float)rt.size.y;
                    }
                }
                else
                {
                    canvas.virtualSize = canvas.referenceSize;
                }

                canvas.mat = glm::ortho<float>(0, canvas.virtualSize.x, 0, canvas.virtualSize.y);
            }
        });

    cubos.system("set canvas children rect")
        .tagged(uiCanvasChildrenUpdateTag)
        .with<UIElement>()
        .withOpt<UIHorizontalStretch>()
        .withOpt<UIVerticalStretch>()
        .withOpt<UINativeAspectRatio>()
        .related<ChildOf>()
        .with<UICanvas>()
        .call([](Query<UIElement&, Opt<const UIHorizontalStretch&>, Opt<const UIVerticalStretch&>,
                       Opt<const UINativeAspectRatio&>, const UICanvas&>
                     query) {
            for (auto [element, hs, vs, nar, canvas] : query)
            {
                element.position = element.anchor * canvas.virtualSize + element.offset;
                if (hs.contains())
                {
                    element.size.x = canvas.virtualSize.x - hs.value().rightOffset - hs.value().leftOffset;
                    element.position.x = hs.value().leftOffset + element.size.x * element.pivot.x + element.offset.x;
                    if (nar.contains() && !vs.contains() && nar.value().ratio != 0)
                    {
                        element.size.y = element.size.x / nar.value().ratio;
                    }
                }
                if (vs.contains())
                {
                    element.size.y = canvas.virtualSize.y - vs.value().topOffset - vs.value().bottomOffset;
                    element.position.y = vs.value().bottomOffset + element.size.y * element.pivot.y + element.offset.y;
                    if (nar.contains() && !hs.contains() && nar.value().ratio != 0)
                    {
                        element.size.x = element.size.y * nar.value().ratio;
                    }
                }
                if (nar.contains() && (vs.contains() == hs.contains()))
                {
                    float currentRatio = element.size.x / element.size.y;
                    if (currentRatio > nar.value().ratio)
                    {
                        element.size.x = element.size.y * nar.value().ratio;
                    }
                    else
                    {
                        element.size.y = element.size.x / nar.value().ratio;
                    }
                }
            }
        });

    cubos.system("set child position")
        .tagged(uiElementPropagateTag)
        .with<UIElement>()
        .withOpt<UIHorizontalStretch>()
        .withOpt<UIVerticalStretch>()
        .withOpt<UINativeAspectRatio>()
        .related<ChildOf>(core::ecs::Traversal::Down)
        .with<UIElement>()
        .call([](Query<UIElement&, Opt<const UIHorizontalStretch&>, Opt<const UIVerticalStretch&>,
                       Opt<const UINativeAspectRatio&>, const UIElement&>
                     children) {
            for (auto [child, hs, vs, nar, parent] : children)
            {
                child.position = child.anchor * parent.topRightCorner() +
                                 (glm::vec2{1, 1} - child.anchor) * parent.bottomLeftCorner() + child.offset;
                child.hierarchyDepth = parent.hierarchyDepth + 1;
                if (hs.contains())
                {
                    child.size.x = parent.topRightCorner().x - hs.value().rightOffset - parent.bottomLeftCorner().x -
                                   hs.value().leftOffset;
                    child.position.x = parent.bottomLeftCorner().x + hs.value().leftOffset +
                                       child.size.x * child.pivot.x + child.offset.x;
                    if (nar.contains() && !vs.contains() && nar.value().ratio != 0)
                    {
                        child.size.y = child.size.x / nar.value().ratio;
                    }
                }
                if (vs.contains())
                {
                    child.size.y = parent.topRightCorner().y - vs.value().topOffset - parent.bottomLeftCorner().y -
                                   vs.value().bottomOffset;
                    child.position.y = parent.bottomLeftCorner().y + vs.value().bottomOffset +
                                       child.size.y * child.pivot.y + child.offset.y;
                    if (nar.contains() && !hs.contains() && nar.value().ratio != 0)
                    {
                        child.size.x = child.size.y * nar.value().ratio;
                    }
                }
                if (nar.contains() && (vs.contains() == hs.contains()))
                {
                    float currentRatio = child.size.x / child.size.y;
                    if (currentRatio > nar.value().ratio)
                    {
                        child.size.x = child.size.y * nar.value().ratio;
                    }
                    else
                    {
                        child.size.y = child.size.x / nar.value().ratio;
                    }
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

    cubos.system("draw ui")
        .tagged(uiEndTag)
        .with<UIElement>()
        .related<ChildOf>()
        .with<UICanvas>()
        .other()
        .with<UIElement>()
        .related<ChildOf>()
        .with<UIElement>()
        .call([](Query<Entity, UIElement&, const UICanvas&> drawsToQuery,
                 Query<Entity, UIElement&, UIElement&> childrenQuery,
                 Query<Entity, UICanvas&, RenderTarget&> canvasQuery, const core::io::Window& window,
                 const State& state) {
            // For each render target with a UI canvas.
            window->renderDevice().setDepthStencilState(state.dss);

            for (auto [entity, canvas, renderTarget] : canvasQuery)
            {
                // Prepare what is common to all draw command types.
                window->renderDevice().setViewport(0, 0, static_cast<int>(renderTarget.size.x),
                                                   static_cast<int>(renderTarget.size.y));
                state.mvpBuffer->fill(&canvas.mat, sizeof(glm::mat4));

                // Extract draw commands from all children UI elements, recursively.
                std::map<const UIDrawList::Type*, UIDrawList> commands;
                for (auto [child, childElement, _] : drawsToQuery.pin(1, entity))
                {
                    copyCommands(childrenQuery, childElement, child, commands);
                }

                // For each draw command type.
                for (const auto& [type, list] : commands)
                {

                    // Prepare state common to all instances of this draw command type.
                    window->renderDevice().setShaderPipeline(type->pipeline);
                    type->pipeline->getBindingPoint("MVP")->bind(state.mvpBuffer);
                    type->constantBufferBindingPoint->bind(type->constantBuffer);

                    // For all instances of this draw command type.
                    for (size_t i = 0; i < list.size(); i++)
                    {
                        // Prepare command-specific state and issue draw call.
                        auto entry = list.entry(i);
                        window->renderDevice().setVertexArray(entry.command.vertexArray);

                        type->constantBuffer->fill(list.data(i), type->perElementSize);

                        for (size_t j = 0; j < UIDrawList::Type::MaxTextures; j++)
                        {
                            if (type->texBindingPoint[j] == nullptr)
                            {
                                continue;
                            }
                            type->texBindingPoint[j]->bind(entry.command.textures[j]);
                            type->texBindingPoint[j]->bind(entry.command.samplers[j]);
                        }
                        window->renderDevice().drawTriangles(entry.command.vertexOffset, entry.command.vertexCount);
                    }
                }
            }
        });
}