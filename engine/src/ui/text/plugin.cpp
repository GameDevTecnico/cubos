#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/font/atlas/atlas.hpp>
#include <cubos/engine/font/font.hpp>
#include <cubos/engine/font/plugin.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/ui/canvas/element.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/text/plugin.hpp>
#include <cubos/engine/ui/text/text.hpp>
#include <cubos/engine/ui/text/text_stretch.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "text_impl.hpp"

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
        int padding[3];
    };

    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        UIDrawList::Type drawType;
        Sampler sampler;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline)
        {
            SamplerDesc sd = {.minFilter = TextureFilter::Linear, .magFilter = TextureFilter::Linear};
            sampler = renderDevice.createSampler(sd);

            drawType.constantBuffer = renderDevice.createConstantBuffer(sizeof(PerElement), nullptr, Usage::Dynamic);
            drawType.texBindingPoint[0] = pipeline->getBindingPoint("fontAtlas");
            drawType.constantBufferBindingPoint = pipeline->getBindingPoint("PerElement");
            drawType.perElementSize = sizeof(PerElement);
            drawType.pipeline = pipeline;
        }
    };
} // namespace

void cubos::engine::uiTextPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("51c11c57-c819-4a51-806c-853178ec686a");
    static const Asset<Shader> PixelShader = AnyAsset("b5b43fcb-0ec3-4f3a-9e90-a7b0b9978cc5");

    cubos.component<UIText>();
    cubos.component<UITextImpl>();
    cubos.component<UITextStretch>();

    cubos.depends(uiCanvasPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(fontPlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("setup UI text")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, const Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps));
        });

    cubos.observer("add UITextImpl on add UIText").onAdd<UIText>().call([](Commands cmds, Query<Entity> query) {
        for (auto [entity] : query)
        {
            cmds.add(entity, UITextImpl{});
        }
    });

    cubos.observer("remove UITextImpl on remove UIText")
        .onRemove<UIText>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [entity] : query)
            {
                cmds.remove<UITextImpl>(entity);
            }
        });

    cubos.system("load UI text vertex array")
        .call([](const Window& window, const State& state, Assets& assets,
                 Query<UIElement&, UIText&, UITextImpl&, Opt<const UITextStretch&>> query) {
            for (auto [element, uiText, textImpl, optStretch] : query)
            {
                if (textImpl.vertexArray == nullptr)
                {
                    if (!uiText.fontAtlas.isStrong())
                    {
                        uiText.fontAtlas = assets.load(uiText.fontAtlas);
                    }
                    auto atlas = assets.read(uiText.fontAtlas);

                    float xPos = 0;
                    float yPos = 0;

                    VertexArrayDesc vad;
                    vad.elementCount = 2;
                    vad.elements[0].name = "in_position";
                    vad.elements[0].type = cubos::core::gl::Type::Float;
                    vad.elements[0].size = 2;
                    vad.elements[0].buffer.index = 0;
                    vad.elements[0].buffer.offset = 0;
                    vad.elements[0].buffer.stride = 4 * sizeof(float);
                    vad.elements[1].name = "in_texCoord";
                    vad.elements[1].type = cubos::core::gl::Type::Float;
                    vad.elements[1].size = 2;
                    vad.elements[1].buffer.index = 0;
                    vad.elements[1].buffer.offset = 2 * sizeof(float);
                    vad.elements[1].buffer.stride = 4 * sizeof(float);
                    vad.shaderPipeline = state.drawType.pipeline;

                    std::vector<float> verts;
                    verts.reserve(4 * 6 * uiText.text.size());
                    /// @todo make add support for Unicode
                    /// @note This repeats a lot of vertices because currently the UI system does not support drawing
                    /// Indexed, something that needs to be worked on
                    for (char c : uiText.text)
                    {
                        auto uChar = static_cast<uint32_t>(static_cast<unsigned char>(c));
                        std::optional<FontGlyph> optGlyph = atlas->requestGlyph(uChar);
                        if (!optGlyph.has_value())
                        {
                            CUBOS_WARN("GLYPH NOT PRESENT!");
                            optGlyph = atlas->requestGlyph(static_cast<uint32_t>('?'));
                        }
                        FontGlyph glyph = optGlyph.value();
                        auto advance = static_cast<float>(glyph.advance * uiText.fontSize);
                        if (c == ' ')
                        {
                            xPos += advance;
                            continue;
                        }
                        float texCoordLeft = glyph.texCoordsMin.x;
                        float texCoordBottom = glyph.texCoordsMin.y;
                        float texCoordRight = glyph.texCoordsMax.x;
                        float texCoordTop = glyph.texCoordsMax.y;

                        float posLeft = glyph.positionMin.x * uiText.fontSize + xPos;
                        float posBottom = glyph.positionMin.y * uiText.fontSize + yPos;
                        float posRight = glyph.positionMax.x * uiText.fontSize + xPos;
                        float posTop = glyph.positionMax.y * uiText.fontSize + yPos;

                        std::array<float, 24> charVerts{
                            posLeft,       posBottom,      texCoordLeft, texCoordBottom, posRight,      posBottom,
                            texCoordRight, texCoordBottom, posLeft,      posTop,         texCoordLeft,  texCoordTop,
                            posLeft,       posTop,         texCoordLeft, texCoordTop,    posRight,      posBottom,
                            texCoordRight, texCoordBottom, posRight,     posTop,         texCoordRight, texCoordTop};
                        /// @note maybe possible to optimize with a memcpy, instead of iterating/pushing_back?
                        for (float f : charVerts)
                        {
                            verts.push_back(f);
                        }
                        xPos += advance;
                    }
                    vad.buffers[0] = window->renderDevice().createVertexBuffer(verts.size() * sizeof(float),
                                                                               verts.data(), Usage::Default);
                    textImpl.vertexArray = window->renderDevice().createVertexArray(vad);
                    textImpl.vertexCount = verts.size() / 4;

                    if (optStretch.contains())
                    {
                        element.size = {xPos, uiText.fontSize};
                    }
                }
            }
        });

    cubos.system("draw UI text")
        .tagged(uiDrawTag)
        .call([](const State& state, const Assets& assets, Query<UIElement&, UIText&, UITextImpl&> query) {
            for (auto [element, uiText, textImpl] : query)
            {
                if (textImpl.vertexArray != nullptr)
                {
                    auto atlas = assets.read(uiText.fontAtlas);
                    glm::vec2 min = element.position - element.size * element.pivot;
                    glm::vec2 max = element.position + element.size * (glm::vec2(1.0F, 1.0F) - element.pivot);
                    element
                        .draw(state.drawType, textImpl.vertexArray, 0, textImpl.vertexCount,
                              PerElement{{min.x, max.x},
                                         {min.y, max.y},
                                         uiText.color,
                                         element.layer * 100 + element.hierarchyDepth,
                                         {}})
                        .withTexture(0, atlas->texture(), state.sampler);
                }
            }
        });
}
