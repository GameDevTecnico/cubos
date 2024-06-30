#include <glm/glm.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/ui/canvas/draw_list.hpp>
#include <cubos/engine/ui/canvas/element.hpp>
#include <cubos/engine/ui/canvas/native_aspect_ratio.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/image/image.hpp>
#include <cubos/engine/ui/image/plugin.hpp>
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
        float padding[3] = {0.0F, 0.0F, 0.0F};
    };

    struct State
    {
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
            drawType.texBindingPoint[0] = pipeline->getBindingPoint("image");
            drawType.constantBufferBindingPoint = pipeline->getBindingPoint("PerElement");
            drawType.perElementSize = sizeof(PerElement);
            drawType.pipeline = pipeline;
        }
    };
} // namespace

void cubos::engine::uiImagePlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("37b485cc-a404-480e-9989-bb79373e354b");
    static const Asset<Shader> PixelShader = AnyAsset("82775d47-0aa4-465f-8e3d-05ea1a9b78c0");

    cubos.component<UIImage>();

    cubos.depends(uiCanvasPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(assetsPlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("setup UI image")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, const Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps));
        });

    cubos.system("set UI Image native aspect ration")
        .before(uiCanvasChildrenUpdateTag)
        .with<UIImage>()
        .with<UINativeAspectRatio>()
        .call([](Query<const UIImage&, UINativeAspectRatio&> query, const Assets& assets) {
            for (auto [image, ratio] : query)
            {
                if (image.texture == nullptr)
                {
                    auto asset = assets.read<Image>(image.asset);
                    ratio.ratio = (float)asset->width / (float)asset->height;
                }
            }
        });

    cubos.system("draw UI images")
        .tagged(uiDrawTag)
        .with<UIElement>()
        .with<UIImage>()
        .call([](const Assets& assets, State& state, const Window& window, Query<UIElement&, UIImage&> query) {
            for (auto [element, image] : query)
            {
                if (image.texture == nullptr)
                {
                    auto asset = assets.read<Image>(image.asset);
                    Texture2DDesc textDesc{.data = {asset->data},
                                           .width = asset->width,
                                           .height = asset->height,
                                           .usage = Usage::Default,
                                           .format = TextureFormat::RGBA8UNorm};
                    image.texture = window->renderDevice().createTexture2D(textDesc);
                }
                glm::vec2 min = element.position - element.size * element.pivot;
                glm::vec2 max = element.position + element.size * (glm::vec2(1.0F, 1.0F) - element.pivot);
                element
                    .draw(
                        state.drawType, state.va, 0, 6,
                        PerElement{
                            {min.x, max.x}, {min.y, max.y}, {1, 1, 1, 1}, element.layer * 100 + element.hierarchyDepth})
                    .withTexture(0, image.texture);
            }
        });
}
