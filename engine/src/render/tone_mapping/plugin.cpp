#include <cubos/core/gl/util.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/hdr/hdr.hpp>
#include <cubos/engine/render/hdr/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::gl::generateScreenQuad;
using cubos::core::gl::RenderDevice;
using cubos::core::gl::ShaderBindingPoint;
using cubos::core::gl::ShaderPipeline;
using cubos::core::gl::VertexArray;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::toneMappingTag);

namespace
{
    struct State
    {
        ShaderPipeline pipeline;
        ShaderBindingPoint gammaBP;
        ShaderBindingPoint exposureBP;
        ShaderBindingPoint hdrBP;
        VertexArray screenQuad;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline)
            : pipeline(pipeline)
        {
            hdrBP = pipeline->getBindingPoint("hdrTexture");
            gammaBP = pipeline->getBindingPoint("gamma");
            exposureBP = pipeline->getBindingPoint("exposure");
            CUBOS_ASSERT(hdrBP && gammaBP && exposureBP, "hdrTexture, gamma and exposure binding points must exist");

            generateScreenQuad(renderDevice, pipeline, screenQuad);
        }
    };
} // namespace

void cubos::engine::toneMappingPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("9532bb85-e1c6-4087-b281-35c41b0aeb68");
    static const Asset<Shader> PixelShader = AnyAsset("478f33d7-cb5c-4487-bc29-4654bda23541");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(hdrPlugin);

    cubos.tag(toneMappingTag).after(drawToHDRTag);

    cubos.uninitResource<State>();

    cubos.component<ToneMapping>();

    cubos.startupSystem("setup Tone Mapping")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps));
        });

    cubos.system("apply Tone Mapping to the HDR texture")
        .tagged(drawToRenderTargetTag)
        .tagged(toneMappingTag)
        .call([](const State& state, const Window& window, Query<RenderTarget&, const HDR&, const ToneMapping&> query) {
            auto& rd = window->renderDevice();

            for (auto [target, hdr, toneMapping] : query)
            {
                rd.setFramebuffer(target.framebuffer);
                rd.setViewport(0, 0, static_cast<int>(target.size.x), static_cast<int>(target.size.y));
                rd.setRasterState(nullptr);
                rd.setBlendState(nullptr);
                rd.setDepthStencilState(nullptr);
                rd.clearDepth(1.0F);
                rd.setShaderPipeline(state.pipeline);
                state.hdrBP->bind(hdr.frontTexture);
                state.gammaBP->setConstant(toneMapping.gamma);
                state.exposureBP->setConstant(toneMapping.exposure);
                rd.setVertexArray(state.screenQuad);
                rd.drawTriangles(0, 6);

                target.cleared = true;
            }
        });
}
