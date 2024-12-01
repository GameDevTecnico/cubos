#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/ui/canvas/draw_list.hpp>
#include <cubos/engine/ui/canvas/element.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/color_rect/color_rect.hpp>
#include <cubos/engine/ui/color_rect/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;

using namespace cubos::engine;

namespace
{
    struct PerElement
    {
        glm::vec2 xRange;
        glm::vec2 yRange;
        glm::vec4 color;
        int depth;
        int padding[3] = {};
    };

    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        VertexArray va;
        UIDrawList::Type drawType;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline)
        {
            VertexArrayDesc vad;
            vad.elementCount = 1;
            vad.elements[0].name = "position";
            vad.elements[0].type = cubos::core::gl::Type::Float;
            vad.elements[0].size = 3;
            vad.elements[0].buffer.index = 0;
            vad.elements[0].buffer.offset = 0;
            vad.elements[0].buffer.stride = 3 * sizeof(float);

            vad.shaderPipeline = pipeline;
            float verts[] = {0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F,
                             0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.0F};
            cubos::core::gl::VertexBuffer vb =
                renderDevice.createVertexBuffer(sizeof(verts), verts, cubos::core::gl::Usage::Dynamic);
            vad.buffers[0] = vb;
            va = renderDevice.createVertexArray(vad);

            drawType.constantBuffer = renderDevice.createConstantBuffer(sizeof(PerElement), nullptr, Usage::Dynamic);

            drawType.constantBufferBindingPoint = pipeline->getBindingPoint("PerElement");
            drawType.perElementSize = sizeof(PerElement);
            drawType.pipeline = pipeline;
        }
    };
} // namespace

void cubos::engine::colorRectPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("37b485cc-a404-480e-9989-bb79373e354b");
    static const Asset<Shader> PixelShader = AnyAsset("478f33d7-cb5c-3387-bc29-4654bda23541");

    cubos.component<UIColorRect>();

    cubos.depends(uiCanvasPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(assetsPlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("setup color rect")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, const Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps));
        });

    cubos.system("draw color rect")
        .tagged(uiDrawTag)
        .with<UIElement>()
        .with<UIColorRect>()
        .call([](State& state, Query<UIElement&, const UIColorRect&> query) {
            for (auto [element, colorRect] : query)
            {
                glm::vec2 min = element.position - element.size * element.pivot;
                glm::vec2 max = element.position + element.size * (glm::vec2(1.0F, 1.0F) - element.pivot);
                element.draw(
                    state.drawType, state.va, 0, 6,
                    PerElement{
                        {min.x, max.x}, {min.y, max.y}, colorRect.color, element.layer * 100 + element.hierarchyDepth});
            }
        });
}