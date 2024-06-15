#include <cmath>

#include <cubos/core/gl/util.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/bloom/bloom.hpp>
#include <cubos/engine/render/bloom/plugin.hpp>
#include <cubos/engine/render/hdr/hdr.hpp>
#include <cubos/engine/render/hdr/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core::gl;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::bloomTag);

namespace
{
    // This implementation is based on the following tutorial:
    // https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/

    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        ShaderPipeline extractPipeline;
        ShaderBindingPoint extractInputBP;
        ShaderBindingPoint extractThresholdFilterBP;
        ShaderBindingPoint extractLuminanceBP;
        VertexArray extractScreenQuad;

        ShaderPipeline bloomPipeline;
        ShaderBindingPoint bloomInputBP;
        ShaderBindingPoint bloomSourceBP;
        ShaderBindingPoint bloomScalingBP;
        ShaderBindingPoint bloomIntensityBP;
        ShaderBindingPoint bloomPassBP;
        VertexArray bloomScreenQuad;

        Sampler sampler;

        BlendState upscalingBlendState;

        State(RenderDevice& renderDevice, const ShaderPipeline& extractPipeline, const ShaderPipeline& bloomPipeline)
            : extractPipeline(extractPipeline)
            , bloomPipeline(bloomPipeline)
        {
            extractInputBP = extractPipeline->getBindingPoint("inputTexture");
            extractThresholdFilterBP = extractPipeline->getBindingPoint("thresholdFilter");
            extractLuminanceBP = extractPipeline->getBindingPoint("luminance");
            CUBOS_ASSERT(extractInputBP && extractThresholdFilterBP && extractLuminanceBP,
                         "inputTexture, thresholdFilter and luminance binding points must exist");
            generateScreenQuad(renderDevice, extractPipeline, extractScreenQuad);

            bloomInputBP = bloomPipeline->getBindingPoint("inputTexture");
            bloomSourceBP = bloomPipeline->getBindingPoint("sourceTexture");
            bloomScalingBP = bloomPipeline->getBindingPoint("scaling");
            bloomIntensityBP = bloomPipeline->getBindingPoint("intensity");
            bloomPassBP = bloomPipeline->getBindingPoint("pass");
            CUBOS_ASSERT(bloomInputBP && bloomSourceBP && bloomScalingBP && bloomIntensityBP && bloomPassBP,
                         "inputTexture, sourceTexture, scaling, intensity, and pass binding points must exist");
            generateScreenQuad(renderDevice, bloomPipeline, bloomScreenQuad);

            sampler = renderDevice.createSampler({
                .minFilter = TextureFilter::Linear,
                .magFilter = TextureFilter::Linear,
                .addressU = AddressMode::Clamp,
                .addressV = AddressMode::Clamp,
            });

            upscalingBlendState = renderDevice.createBlendState({
                .blendEnabled = true,
                .color = {.src = BlendFactor::One, .dst = BlendFactor::One},
                .alpha = {.src = BlendFactor::One},
            });
        }
    };
} // namespace

void cubos::engine::bloomPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("9532bb85-e1c6-4087-b281-35c41b0aeb68");
    static const Asset<Shader> ExtractPixelShader = AnyAsset("ef996f7b-dae0-4b4d-8da2-ad98bf7dee1f");
    static const Asset<Shader> BloomPixelShader = AnyAsset("7ee2c393-1512-4077-abaa-8d5adbb10222");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(hdrPlugin);

    cubos.tag(bloomTag).tagged(drawToHDRTag);

    cubos.uninitResource<State>();

    cubos.component<Bloom>();

    cubos.startupSystem("setup Bloom")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto extractPS = assets.read(ExtractPixelShader)->shaderStage();
            auto bloomPS = assets.read(BloomPixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, extractPS),
                                        rd.createShaderPipeline(vs, bloomPS));
        });

    cubos.system("apply Bloom to the HDR texture")
        .tagged(bloomTag)
        .call([](const State& state, const Window& window, Query<HDR&, Bloom&> targets) {
            auto& rd = window->renderDevice();

            for (auto [hdr, bloom] : targets)
            {
                // Skip if the HDR texture is not valid.
                if (hdr.size.x == 0 || hdr.size.y == 0)
                {
                    continue;
                }

                // Check if the HDR textures have changed.
                if (bloom.frontHDR != hdr.frontTexture)
                {
                    // It may have just been swapped by a post-processing effect.
                    std::swap(bloom.frontFramebuffer, bloom.backFramebuffer);
                    std::swap(bloom.frontHDR, bloom.backHDR);
                }
                if (bloom.frontHDR != hdr.frontTexture)
                {
                    // Then the texture has really changed. Create a new framebuffer for drawing to the new HDR
                    // texture.
                    FramebufferDesc desc{};
                    desc.targetCount = 1;
                    desc.targets[0].setTexture2DTarget(hdr.frontTexture);
                    bloom.frontFramebuffer = rd.createFramebuffer(desc);
                    bloom.frontHDR = hdr.frontTexture;
                    desc.targets[0].setTexture2DTarget(hdr.backTexture);
                    bloom.backFramebuffer = rd.createFramebuffer(desc);
                    bloom.backHDR = hdr.backTexture;

                    CUBOS_INFO("Recreated Bloom HDR framebuffers");
                }

                // Check if the HDR texture size has changed.
                Texture2DDesc texDesc{
                    .width = hdr.size.x,
                    .height = hdr.size.y,
                    .usage = Usage::Dynamic,
                    .format = TextureFormat::RGBA32Float,
                };
                if (bloom.size != hdr.size)
                {
                    bloom.size = hdr.size;

                    // Recreate the extraction texture and framebuffer.
                    bloom.extractionTexture = rd.createTexture2D(texDesc);
                    FramebufferDesc fbDesc{};
                    fbDesc.targetCount = 1;
                    fbDesc.targets[0].setTexture2DTarget(bloom.extractionTexture);
                    bloom.extractionFramebuffer = rd.createFramebuffer(fbDesc);

                    // Force the bloom textures and framebuffers to be recreated.
                    bloom.bloomTextures.clear();
                    bloom.bloomFramebuffers.clear();
                }

                // Create bloom textures and framebuffers if they don't exist.
                for (std::size_t i = 0; i < bloom.bloomTextures.size(); ++i)
                {
                    texDesc.width /= 2;
                    texDesc.height /= 2;
                }
                for (std::size_t i = bloom.bloomTextures.size(); i < static_cast<std::size_t>(bloom.iterations); ++i)
                {
                    texDesc.width /= 2;
                    texDesc.height /= 2;
                    if (texDesc.width == 0 || texDesc.height == 0)
                    {
                        // The number of iterations is too high for the texture size.
                        // Simply do not create any more bloom textures.
                        break;
                    }

                    bloom.bloomTextures.emplace_back(rd.createTexture2D(texDesc));
                    FramebufferDesc fbDesc{};
                    fbDesc.targetCount = 1;
                    fbDesc.targets[0].setTexture2DTarget(bloom.bloomTextures.back());
                    bloom.bloomFramebuffers.emplace_back(rd.createFramebuffer(fbDesc));
                }

                // Skip if no bloom textures were created.
                if (bloom.bloomTextures.empty())
                {
                    continue;
                }

                // Set the necessary state.
                rd.setViewport(0, 0, static_cast<int>(bloom.size.x), static_cast<int>(bloom.size.y));
                rd.setRasterState(nullptr);
                rd.setBlendState(nullptr);
                rd.setDepthStencilState(nullptr);

                // Calculate the needed threshold values so that we can submit them packed as a vec4 to the shader.
                float knee = bloom.threshold * bloom.softThreshold;
                glm::vec4 thresholdFilter{
                    bloom.threshold,
                    bloom.threshold - knee,
                    knee * 2.0F,
                    0.25F / (knee + 0.0001F),
                };

                // Extract the bright parts of the HDR texture.
                rd.setFramebuffer(bloom.extractionFramebuffer);
                rd.clearColor(0.0F, 0.0F, 0.0F, 0.0F);
                rd.setShaderPipeline(state.extractPipeline);
                state.extractInputBP->bind(hdr.frontTexture);
                state.extractInputBP->bind(state.sampler);
                state.extractThresholdFilterBP->setConstant(thresholdFilter);
                state.extractLuminanceBP->setConstant(bloom.luminance);
                rd.setVertexArray(state.extractScreenQuad);
                rd.drawTriangles(0, 6);

                // Downscale the extracted texture.
                rd.setShaderPipeline(state.bloomPipeline);
                rd.setVertexArray(state.bloomScreenQuad);
                state.bloomInputBP->bind(bloom.extractionTexture);
                state.bloomInputBP->bind(state.sampler);
                state.bloomPassBP->setConstant(0); // DOWNSCALE_PASS

                float scaling = 2.0F;
                for (std::size_t i = 0; i < bloom.bloomTextures.size(); ++i)
                {
                    state.bloomScalingBP->setConstant(scaling);
                    rd.setFramebuffer(bloom.bloomFramebuffers[i]);
                    rd.clearColor(0.0F, 0.0F, 0.0F, 0.0F);
                    rd.drawTriangles(0, 6);
                    state.bloomInputBP->bind(bloom.bloomTextures[i]);
                    scaling *= 2.0F;
                }

                // Upscale the bloom textures additively.
                scaling /= 2.0F;
                state.bloomPassBP->setConstant(1); // UPSCALE_PASS
                rd.setBlendState(state.upscalingBlendState);
                for (std::size_t i = bloom.bloomTextures.size() - 1; i-- > 0;)
                {
                    scaling /= 2.0F;
                    state.bloomScalingBP->setConstant(scaling);
                    rd.setFramebuffer(bloom.bloomFramebuffers[i]);
                    rd.drawTriangles(0, 6);
                    state.bloomInputBP->bind(bloom.bloomTextures[i]);
                }

                // Combine the final bloom texture with the HDR texture.
                scaling /= 2.0F;
                float linearIntensity = powf(bloom.intensity, 1.0F / 2.2F); // Convert from gamma to linear space
                state.bloomScalingBP->setConstant(scaling);
                state.bloomIntensityBP->setConstant(linearIntensity);
                state.bloomPassBP->setConstant(2); // COMBINE_PASS
                state.bloomSourceBP->bind(hdr.frontTexture);
                state.bloomSourceBP->bind(state.sampler);
                rd.setBlendState(nullptr);
                rd.setFramebuffer(bloom.backFramebuffer);
                rd.clearColor(0.0F, 0.0F, 0.0F, 0.0F);
                rd.drawTriangles(0, 6);

                // Swap the front and back HDR textures.
                std::swap(hdr.frontTexture, hdr.backTexture);
            }
        });
}
