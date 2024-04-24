#include <cubos/core/gl/util.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective_camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/deferred_shading/deferred_shading.hpp>
#include <cubos/engine/render/deferred_shading/plugin.hpp>
#include <cubos/engine/render/g_buffer/g_buffer.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/hdr/hdr.hpp>
#include <cubos/engine/render/hdr/plugin.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/lights/point.hpp>
#include <cubos/engine/render/lights/spot.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::gl::ConstantBuffer;
using cubos::core::gl::FramebufferDesc;
using cubos::core::gl::generateScreenQuad;
using cubos::core::gl::RenderDevice;
using cubos::core::gl::ShaderBindingPoint;
using cubos::core::gl::ShaderPipeline;
using cubos::core::gl::Usage;
using cubos::core::gl::VertexArray;
using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::deferredShadingTag);

namespace
{
    struct PerDirectionalLight
    {
        glm::vec4 direction;
        glm::vec4 color;
        float intensity;
    };

    struct PerPointLight
    {
        glm::vec4 position;
        glm::vec4 color;
        float intensity;
    };

    struct PerSpotLight
    {
        glm::vec4 position;
        glm::vec4 direction;
        glm::vec4 color;
        float intensity;
        float range;
        float spotCutoff;
        float innerSpotCutoff;
    };

    struct PerScene
    {
        glm::mat4 inverseView;
        glm::mat4 inverseProjection;

        glm::vec4 skyGradient[2];
        glm::vec4 ambientLight;

        glm::uint numDirectionalLights{0};
        PerDirectionalLight directionalLights[16];

        glm::uint numPointLights{0};
        PerPointLight pointLights[128];

        glm::uint numSpotLights{0};
        PerSpotLight spotLights[128];
    };

    struct State
    {
        ShaderPipeline pipeline;
        ShaderBindingPoint positionBP;
        ShaderBindingPoint normalBP;
        ShaderBindingPoint albedoBP;
        ShaderBindingPoint perSceneBP;

        VertexArray screenQuad;

        ConstantBuffer perSceneCB;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline)
            : pipeline(pipeline)
        {
            positionBP = pipeline->getBindingPoint("positionTexture");
            normalBP = pipeline->getBindingPoint("normalTexture");
            albedoBP = pipeline->getBindingPoint("albedoTexture");
            perSceneBP = pipeline->getBindingPoint("PerScene");
            CUBOS_ASSERT(positionBP && normalBP && albedoBP && perSceneBP,
                         "positionTexture, normalTexture, albedoTexture and PerScene binding points must exist");

            generateScreenQuad(renderDevice, pipeline, screenQuad);

            perSceneCB = renderDevice.createConstantBuffer(sizeof(PerScene), nullptr, Usage::Dynamic);
        }
    };
} // namespace

void cubos::engine::deferredShadingPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("9532bb85-e1c6-4087-b281-35c41b0aeb68");
    static const Asset<Shader> PixelShader = AnyAsset("ec97a5af-e9ae-46c6-bbf2-6c40cbe2711b");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(gBufferPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(lightsPlugin);
    cubos.depends(hdrPlugin);
    cubos.depends(transformPlugin);

    cubos.tag(deferredShadingTag).tagged(drawToHDRTag).after(drawToGBufferTag);

    cubos.uninitResource<State>();

    cubos.component<DeferredShading>();

    cubos.startupSystem("setup Deferred Shading")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps));
        });

    cubos.system("apply Deferred Shading to the GBuffer and output to the HDR texture")
        .tagged(deferredShadingTag)
        .call([](const State& state, const Window& window, const RenderEnvironment& environment,
                 Query<const LocalToWorld&, const DirectionalLight&> directionalLights,
                 Query<const LocalToWorld&, const PointLight&> pointLights,
                 Query<const LocalToWorld&, const SpotLight&> spotLights,
                 Query<Entity, const HDR&, const GBuffer&, DeferredShading&> targets,
                 Query<const LocalToWorld&, const PerspectiveCamera&, const DrawsTo&> perspectiveCameras) {
            auto& rd = window->renderDevice();

            for (auto [targetEnt, hdr, gBuffer, deferredShading] : targets)
            {
                // Find the camera that draws to the GBuffer.
                for (auto [localToWorld, camera, drawsTo] : perspectiveCameras.pin(1, targetEnt))
                {
                    if (!camera.active)
                    {
                        continue;
                    }

                    // Check if the HDR texture has changed.
                    if (deferredShading.hdr != hdr.texture)
                    {
                        // Create a new framebuffer for drawing to the new HDR texture.
                        FramebufferDesc desc{};
                        desc.targetCount = 1;
                        desc.targets[0].setTexture2DTarget(hdr.texture);
                        deferredShading.framebuffer = rd.createFramebuffer(desc);
                        deferredShading.hdr = hdr.texture;

                        CUBOS_INFO("Recreated Deferred Shading framebuffer");
                    }

                    // Fill the PerScene constant buffer.
                    PerScene perScene{};
                    perScene.inverseView = localToWorld.mat;
                    perScene.inverseProjection = glm::inverse(
                        glm::perspective(glm::radians(camera.fovY), float(gBuffer.size.x) / float(gBuffer.size.y),
                                         camera.zNear, camera.zFar));

                    perScene.ambientLight = glm::vec4(environment.ambient, 1.0F);
                    perScene.skyGradient[0] = glm::vec4(environment.skyGradient[0], 1.0F);
                    perScene.skyGradient[1] = glm::vec4(environment.skyGradient[1], 1.0F);

                    for (auto [lightLocalToWorld, light] : directionalLights)
                    {
                        auto& perLight = perScene.directionalLights[perScene.numDirectionalLights++];
                        perLight.direction = glm::vec4(0.0F, 0.0F, 1.0F, 0.0F) * lightLocalToWorld.mat;
                        perLight.color = glm::vec4(light.color, 1.0F);
                        perLight.intensity = light.intensity;
                    }

                    for (auto [lightLocalToWorld, light] : pointLights)
                    {
                        auto& perLight = perScene.pointLights[perScene.numPointLights++];
                        perLight.position = glm::vec4(0.0F, 0.0F, 0.0F, 1.0F) * lightLocalToWorld.mat;
                        perLight.color = glm::vec4(light.color, 1.0F);
                        perLight.intensity = light.intensity;
                    }

                    for (auto [lightLocalToWorld, light] : spotLights)
                    {
                        auto& perLight = perScene.spotLights[perScene.numSpotLights++];
                        perLight.position = glm::vec4(0.0F, 0.0F, 0.0F, 1.0F) * lightLocalToWorld.mat;
                        perLight.direction = glm::vec4(0.0F, 0.0F, 1.0F, 0.0F) * lightLocalToWorld.mat;
                        perLight.color = glm::vec4(light.color, 1.0F);
                        perLight.intensity = light.intensity;
                        perLight.range = light.range;
                        perLight.spotCutoff = glm::cos(glm::radians(light.spotAngle));
                        perLight.innerSpotCutoff = glm::cos(glm::radians(light.innerSpotAngle));
                    }

                    state.perSceneCB->fill(&perScene, sizeof(PerScene));

                    // Draw the screen quad with the GBuffer textures.
                    rd.setFramebuffer(deferredShading.framebuffer);
                    rd.setViewport(0, 0, static_cast<int>(gBuffer.size.x), static_cast<int>(gBuffer.size.y));
                    rd.setRasterState(nullptr);
                    rd.setBlendState(nullptr);
                    rd.setDepthStencilState(nullptr);
                    rd.setShaderPipeline(state.pipeline);
                    state.positionBP->bind(gBuffer.position);
                    state.normalBP->bind(gBuffer.normal);
                    state.albedoBP->bind(gBuffer.albedo);
                    state.perSceneBP->bind(state.perSceneCB);
                    rd.setVertexArray(state.screenQuad);
                    rd.drawTriangles(0, 6);

                    // We only use the first active camera, thus, break out of the loop immediately.
                    break;
                }
            }
        });
}
