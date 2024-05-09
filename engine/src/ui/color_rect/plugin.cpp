#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
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
        int depth;
    };
    struct State
    {
        ShaderPipeline pipeline;
        ShaderBindingPoint color;
        VertexArray va;
        VertexArrayDesc vad;
        DepthStencilState dss;
        ConstantBuffer perElement;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline)
            : pipeline(pipeline)
        {
            color = pipeline->getBindingPoint("color");
            CUBOS_ASSERT(color, "color binding point must exist");
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
            cubos::core::gl::DepthStencilStateDesc dssd;
            dssd.depth.enabled = true;
            dssd.depth.writeEnabled = true;
            dssd.depth.near = -1.0F;
            dssd.depth.compare = Compare::LEqual;
            dss = renderDevice.createDepthStencilState(dssd);
            va = renderDevice.createVertexArray(vad);

            perElement = renderDevice.createConstantBuffer(sizeof(PerElement), nullptr, Usage::Dynamic);
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
        .call([](const State& state, const Window& window, Query<UIElement&, const UIColorRect&> query) {
            window->renderDevice().setDepthStencilState(state.dss);
            window->renderDevice().setViewport(0, 0, static_cast<int>(window->framebufferSize()[0]),
                                               static_cast<int>(window->framebufferSize()[1]));
            window->renderDevice().setVertexArray(state.va);
            window->renderDevice().setShaderPipeline(state.pipeline);
            for (auto [element, colorRect] : query)
            {
                float minX = element.position.x - element.size.x * (element.pivot.x);
                float minY = element.position.y - element.size.y * (element.pivot.y);
                float maxX = element.position.x + element.size.x * (1.0F - element.pivot.x);
                float maxY = element.position.y + element.size.y * (1.0F - element.pivot.y);

                PerElement perElement{{minX, maxX}, {minY, maxY}, element.layer * 100 + element.hierarchyDepth};
                state.perElement->fill(&perElement, sizeof(perElement));
                state.pipeline->getBindingPoint("PerElement")->bind(state.perElement);

                auto mvpBuffer = window->renderDevice().createConstantBuffer(sizeof(glm::mat4), &element.vp,
                                                                             cubos::core::gl::Usage::Static);
                state.pipeline->getBindingPoint("MVP")->bind(mvpBuffer);
                state.color->setConstant(colorRect.color);
                window->renderDevice().drawTriangles(0, 6);
            }
        });
}