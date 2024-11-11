#include <cmath>
#include <random>

#include <cubos/core/gl/util.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/active/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/g_buffer/g_buffer.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/ssao/plugin.hpp>
#include <cubos/engine/render/ssao/ssao.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core::gl;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::drawToSSAOTag);

#define MAX_KERNEL_SIZE 64

namespace
{
    struct PerScene
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec4 samples[MAX_KERNEL_SIZE];
        int kernelSize;
        float radius;
        float bias;
    };

    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        ShaderPipeline basePipeline;
        ShaderBindingPoint basePositionBP;
        ShaderBindingPoint baseNormalBP;
        ShaderBindingPoint baseNoiseBP;
        ShaderBindingPoint basePerSceneBP;
        VertexArray baseScreenQuad;
        ShaderBindingPoint baseViewportOffsetBP;
        ShaderBindingPoint baseViewportSizeBP;

        ShaderPipeline blurPipeline;
        ShaderBindingPoint blurSSAOBP;
        ShaderBindingPoint blurViewportOffsetBP;
        ShaderBindingPoint blurViewportSizeBP;
        VertexArray blurScreenQuad;

        ConstantBuffer perSceneCB;
        Texture2D noiseTexture;

        std::vector<glm::vec3> kernel{};

        State(RenderDevice& renderDevice, const ShaderPipeline& basePipeline, const ShaderPipeline& blurPipeline)
            : basePipeline(basePipeline)
            , blurPipeline(blurPipeline)
        {
            basePositionBP = basePipeline->getBindingPoint("positionTexture");
            baseNormalBP = basePipeline->getBindingPoint("normalTexture");
            baseNoiseBP = basePipeline->getBindingPoint("noiseTexture");
            basePerSceneBP = basePipeline->getBindingPoint("PerScene");
            baseViewportOffsetBP = basePipeline->getBindingPoint("viewportOffset");
            baseViewportSizeBP = basePipeline->getBindingPoint("viewportSize");
            CUBOS_ASSERT(basePositionBP && baseNormalBP && baseNoiseBP && basePerSceneBP && baseViewportOffsetBP &&
                             baseViewportSizeBP,
                         "positionTexture, normalTexture, noiseTexture and PerScene, viewportOffset and viewportSize "
                         "binding points must exist");
            generateScreenQuad(renderDevice, basePipeline, baseScreenQuad);

            blurSSAOBP = blurPipeline->getBindingPoint("ssaoTexture");
            blurViewportOffsetBP = blurPipeline->getBindingPoint("viewportOffset");
            blurViewportSizeBP = blurPipeline->getBindingPoint("viewportSize");
            CUBOS_ASSERT(blurSSAOBP && blurViewportOffsetBP && blurViewportSizeBP,
                         "ssaoTexture, viewportOffset and viewportSize binding points must exist");
            generateScreenQuad(renderDevice, blurPipeline, blurScreenQuad);

            perSceneCB = renderDevice.createConstantBuffer(sizeof(PerScene), nullptr, Usage::Dynamic);

            // Generate noise texture
            std::uniform_real_distribution<float> randomFloats(0.0F, 1.0F); // random floats between [0.0, 1.0]
            std::default_random_engine generator;
            std::vector<glm::vec3> ssaoNoise;
            ssaoNoise.resize(16);
            for (unsigned int i = 0; i < 16; i++)
            {
                glm::vec3 noise(randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                                randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                                0.0F);
                ssaoNoise[i] = noise;
            }

            Texture2DDesc desc{};
            desc.width = desc.height = 4;
            desc.format = TextureFormat::RGB32Float;
            desc.data[0] = ssaoNoise.data();
            noiseTexture = renderDevice.createTexture2D(desc);
        }
    };
} // namespace

void cubos::engine::ssaoPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("9532bb85-e1c6-4087-b281-35c41b0aeb68");
    static const Asset<Shader> BasePixelShader = AnyAsset("13fe474a-28cc-4688-a5fd-6a2c08049c0d");
    static const Asset<Shader> BlurPixelShader = AnyAsset("f4d4dbaa-a9c9-440d-b4be-8fc676f5204c");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(gBufferPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(transformPlugin);

    cubos.tag(drawToSSAOTag).after(drawToGBufferTag);

    cubos.uninitResource<State>();

    cubos.component<SSAO>();

    cubos.startupSystem("setup SSAO")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto basePS = assets.read(BasePixelShader)->shaderStage();
            auto blurPS = assets.read(BlurPixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, basePS), rd.createShaderPipeline(vs, blurPS));
        });

    cubos.system("apply SSAO to the GBuffer and output to the SSAO texture")
        .tagged(drawToSSAOTag)
        .call([](State& state, const Window& window, Query<Entity, const GBuffer&, SSAO&> targets,
                 Query<const LocalToWorld&, const Camera&, const Active&, const DrawsTo&> cameras) {
            auto& rd = window->renderDevice();

            for (auto [targetEnt, gBuffer, ssao] : targets)
            {
                // Clamp sample count to a valid range.
                if (ssao.samples > MAX_KERNEL_SIZE)
                {
                    CUBOS_WARN("SSAO sample count is too large, clamping to {}", MAX_KERNEL_SIZE);
                    ssao.samples = MAX_KERNEL_SIZE;
                }
                else if (ssao.samples < 1)
                {
                    CUBOS_WARN("SSAO sample count is too small, clamping to 1");
                    ssao.samples = 1;
                }

                // Generate kernel if the size changed.
                if (state.kernel.size() != static_cast<std::size_t>(ssao.samples))
                {
                    std::uniform_real_distribution<float> randomFloats(0.0F, 1.0F);
                    std::default_random_engine generator{};

                    state.kernel.resize(static_cast<std::size_t>(ssao.samples));
                    for (int i = 0; i < ssao.samples; ++i)
                    {
                        glm::vec3 sample(randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                                         randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                                         randomFloats(generator)                // [ 0.0, 1.0]
                        );
                        sample = glm::normalize(sample);
                        sample *= randomFloats(generator);
                        float scale = static_cast<float>(i) / static_cast<float>(ssao.samples);

                        // Scale samples so that they are more aligned to the center of the kernel.
                        scale = std::lerp(0.1F, 1.0F, scale * scale);
                        sample *= scale;
                        state.kernel[static_cast<std::size_t>(i)] = sample;
                    }
                }

                // Find the cameras that draw to the SSAO target.
                for (auto [localToWorld, camera, active, drawsTo] : cameras.pin(1, targetEnt))
                {
                    if (!active.active)
                    {
                        continue;
                    }

                    // Check if we need to resize the SSAO textures.
                    if (ssao.size != gBuffer.size)
                    {
                        ssao.size = gBuffer.size;

                        // Prepare common texture description.
                        Texture2DDesc desc{};
                        desc.format = TextureFormat::R32Float;
                        desc.width = gBuffer.size.x;
                        desc.height = gBuffer.size.y;
                        desc.usage = Usage::Dynamic;

                        // Create base and blur textures.
                        ssao.baseTexture = rd.createTexture2D(desc);
                        ssao.blurTexture = rd.createTexture2D(desc);

                        // Create framebuffers.
                        FramebufferDesc fbDesc{};
                        fbDesc.targetCount = 1;
                        fbDesc.targets[0].setTexture2DTarget(ssao.baseTexture);
                        ssao.baseFramebuffer = rd.createFramebuffer(fbDesc);
                        fbDesc.targets[0].setTexture2DTarget(ssao.blurTexture);
                        ssao.blurFramebuffer = rd.createFramebuffer(fbDesc);

                        CUBOS_INFO("Resized SSAO textures to {}x{}", ssao.size.x, ssao.size.y);
                    }

                    // Fill the PerScene constant buffer.
                    PerScene perScene{};
                    perScene.view = glm::inverse(localToWorld.mat);
                    perScene.projection = camera.projection;
                    for (std::size_t i = 0; i < state.kernel.size(); ++i)
                    {
                        perScene.samples[i] = glm::vec4(state.kernel[i], 0.0F);
                    }
                    perScene.kernelSize = ssao.samples;
                    perScene.radius = ssao.radius;
                    perScene.bias = ssao.bias;

                    state.perSceneCB->fill(&perScene, sizeof(PerScene));

                    // Set state common to both passes.
                    rd.setViewport(static_cast<int>(drawsTo.viewportOffset.x * float(ssao.size.x)),
                                   static_cast<int>(drawsTo.viewportOffset.y * float(ssao.size.y)),
                                   static_cast<int>(drawsTo.viewportSize.x * float(ssao.size.x)),
                                   static_cast<int>(drawsTo.viewportSize.y * float(ssao.size.y)));
                    rd.setScissor(static_cast<int>(drawsTo.viewportOffset.x * float(ssao.size.x)),
                                  static_cast<int>(drawsTo.viewportOffset.y * float(ssao.size.y)),
                                  static_cast<int>(drawsTo.viewportSize.x * float(ssao.size.x)),
                                  static_cast<int>(drawsTo.viewportSize.y * float(ssao.size.y)));
                    rd.setRasterState(nullptr);
                    rd.setBlendState(nullptr);
                    rd.setDepthStencilState(nullptr);

                    // Base SSAO pass.
                    rd.setFramebuffer(ssao.baseFramebuffer);
                    rd.setShaderPipeline(state.basePipeline);
                    state.basePositionBP->bind(gBuffer.position);
                    state.baseNormalBP->bind(gBuffer.normal);
                    state.baseNoiseBP->bind(state.noiseTexture);
                    state.basePerSceneBP->bind(state.perSceneCB);
                    state.baseViewportOffsetBP->setConstant(drawsTo.viewportOffset);
                    state.baseViewportSizeBP->setConstant(drawsTo.viewportSize);
                    rd.setVertexArray(state.baseScreenQuad);
                    rd.drawTriangles(0, 6);

                    // Blur SSAO pass.
                    rd.setFramebuffer(ssao.blurFramebuffer);
                    rd.setShaderPipeline(state.blurPipeline);
                    state.blurSSAOBP->bind(ssao.baseTexture);
                    state.blurViewportOffsetBP->setConstant(drawsTo.viewportOffset);
                    state.blurViewportSizeBP->setConstant(drawsTo.viewportSize);
                    rd.setVertexArray(state.blurScreenQuad);
                    rd.drawTriangles(0, 6);
                }
            }
        });
}
