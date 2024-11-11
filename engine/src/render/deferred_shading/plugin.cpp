#include <cubos/core/geom/utils.hpp>
#include <cubos/core/gl/util.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/active/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/cascaded_shadow_maps/plugin.hpp>
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
#include <cubos/engine/render/shadow_atlas/plugin.hpp>
#include <cubos/engine/render/shadow_atlas/shadow_atlas.hpp>
#include <cubos/engine/render/shadows/directional_caster.hpp>
#include <cubos/engine/render/shadows/plugin.hpp>
#include <cubos/engine/render/shadows/spot_caster.hpp>
#include <cubos/engine/render/ssao/plugin.hpp>
#include <cubos/engine/render/ssao/ssao.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::gl::AddressMode;
using cubos::core::gl::ConstantBuffer;
using cubos::core::gl::FramebufferDesc;
using cubos::core::gl::generateScreenQuad;
using cubos::core::gl::RenderDevice;
using cubos::core::gl::Sampler;
using cubos::core::gl::SamplerDesc;
using cubos::core::gl::ShaderBindingPoint;
using cubos::core::gl::ShaderPipeline;
using cubos::core::gl::Texture2DArray;
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
        glm::mat4 matrices[cubos::engine::DirectionalShadowCaster::MaxCascades];
        float intensity;
        float shadowBias;
        float shadowBlurRadius;
        float padding[1];
        glm::vec4 shadowFarSplitDistances[(cubos::engine::DirectionalShadowCaster::MaxCascades + 3) /
                                          4]; // intended to be a float array, but std140 layout aligns array elements
                                              // to vec4 size; number of vec4s = ceiling(MaxCascades / 4 components)
        int numCascades;
        int padding2[3];
    };

    struct PerPointLight
    {
        glm::vec4 position;
        glm::vec4 color;
        float intensity;
        float range;
        float padding[2];
    };

    struct PerSpotLight
    {
        glm::vec4 position;
        glm::vec4 direction;
        glm::mat4 matrix;
        glm::vec4 color;
        float intensity;
        float range;
        float spotCutoff;
        float innerSpotCutoff;
        glm::vec2 shadowMapOffset;
        glm::vec2 shadowMapSize;
        float shadowBias;
        float shadowBlurRadius;
        float padding[2];
    };

    struct PerScene
    {
        glm::mat4 inverseView;
        glm::mat4 inverseProjection;

        glm::vec4 skyGradient[2];
        glm::vec4 ambientLight;

        PerDirectionalLight directionalLights[16];
        PerPointLight pointLights[128];
        PerSpotLight spotLights[128];

        glm::uint numDirectionalLights{0};
        glm::uint numPointLights{0};
        glm::uint numSpotLights{0};

        int directionalLightWithShadowsId;
    };

    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        ShaderPipeline pipeline;
        ShaderBindingPoint positionBP;
        ShaderBindingPoint normalBP;
        ShaderBindingPoint albedoBP;
        ShaderBindingPoint ssaoBP;
        ShaderBindingPoint shadowAtlasBP;
        ShaderBindingPoint directionalShadowMapBP;
        ShaderBindingPoint perSceneBP;
        ShaderBindingPoint viewportOffsetBP;
        ShaderBindingPoint viewportSizeBP;
        Sampler directionalShadowSampler;

        VertexArray screenQuad;

        ConstantBuffer perSceneCB;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline)
            : pipeline(pipeline)
        {
            positionBP = pipeline->getBindingPoint("positionTexture");
            normalBP = pipeline->getBindingPoint("normalTexture");
            albedoBP = pipeline->getBindingPoint("albedoTexture");
            ssaoBP = pipeline->getBindingPoint("ssaoTexture");
            shadowAtlasBP = pipeline->getBindingPoint("shadowAtlasTexture");
            directionalShadowMapBP = pipeline->getBindingPoint("directionalShadowMap");
            perSceneBP = pipeline->getBindingPoint("PerScene");
            viewportOffsetBP = pipeline->getBindingPoint("viewportOffset");
            viewportSizeBP = pipeline->getBindingPoint("viewportSize");
            CUBOS_ASSERT(positionBP && normalBP && albedoBP && ssaoBP && shadowAtlasBP && directionalShadowMapBP &&
                             perSceneBP && viewportOffsetBP && viewportSizeBP,
                         "positionTexture, normalTexture, albedoTexture, ssaoTexture, shadowAtlasTexture, "
                         "directionalShadowMap, PerScene, "
                         "viewportOffset and "
                         "viewportSize binding points must exist");

            SamplerDesc directionalShadowSamplerDesc{};
            directionalShadowSamplerDesc.addressU = AddressMode::Border;
            directionalShadowSamplerDesc.addressV = AddressMode::Border;
            for (float& borderColor : directionalShadowSamplerDesc.borderColor)
            {
                borderColor = 1.0F;
            }
            directionalShadowSampler = renderDevice.createSampler(directionalShadowSamplerDesc);

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
    cubos.depends(ssaoPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(lightsPlugin);
    cubos.depends(hdrPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(shadowsPlugin);
    cubos.depends(shadowAtlasPlugin);
    cubos.depends(cascadedShadowMapsPlugin);

    cubos.tag(deferredShadingTag)
        .tagged(drawToHDRTag)
        .after(drawToGBufferTag)
        .after(drawToSSAOTag)
        .after(drawToShadowAtlasTag)
        .after(drawToCascadedShadowMapsTag);

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
        .call([](State& state, const Window& window, const RenderEnvironment& environment,
                 const ShadowAtlas& shadowAtlas,
                 Query<const LocalToWorld&, const DirectionalLight&, const Active&, Opt<const DirectionalShadowCaster&>>
                     directionalLights,
                 Query<const LocalToWorld&, const PointLight&, const Active&> pointLights,
                 Query<const LocalToWorld&, const SpotLight&, const Active&, Opt<const SpotShadowCaster&>> spotLights,
                 Query<Entity, const HDR&, const GBuffer&, const SSAO&, DeferredShading&> targets,
                 Query<Entity, const LocalToWorld&, const Camera&, const Active&, const DrawsTo&> cameras) {
            auto& rd = window->renderDevice();

            for (auto [targetEnt, hdr, gBuffer, ssao, deferredShading] : targets)
            {
                // Find the cameras that draw to the GBuffer.
                for (auto [cameraEntity, localToWorld, camera, cameraActive, drawsTo] : cameras.pin(1, targetEnt))
                {
                    if (!cameraActive.active)
                    {
                        continue;
                    }

                    // Check if the HDR texture has changed.
                    if (deferredShading.frontHDR != hdr.frontTexture)
                    {
                        // It may have just been swapped by a post-processing effect.
                        std::swap(deferredShading.frontFramebuffer, deferredShading.backFramebuffer);
                        std::swap(deferredShading.frontHDR, deferredShading.backHDR);
                    }
                    if (deferredShading.frontHDR != hdr.frontTexture)
                    {
                        // Then the texture has really changed. Create a new framebuffer for drawing to the new HDR
                        // texture.
                        FramebufferDesc desc{};
                        desc.targetCount = 1;
                        desc.targets[0].setTexture2DTarget(hdr.frontTexture);
                        deferredShading.frontFramebuffer = rd.createFramebuffer(desc);
                        deferredShading.frontHDR = hdr.frontTexture;

                        CUBOS_INFO("Recreated Deferred Shading front framebuffer");
                    }

                    // Fill the PerScene constant buffer.
                    PerScene perScene{};
                    perScene.inverseView = localToWorld.mat;
                    perScene.inverseProjection = glm::inverse(camera.projection);
                    perScene.directionalLightWithShadowsId = -1;

                    perScene.ambientLight = glm::vec4(environment.ambient, 1.0F);
                    perScene.skyGradient[0] = glm::vec4(environment.skyGradient[0], 1.0F);
                    perScene.skyGradient[1] = glm::vec4(environment.skyGradient[1], 1.0F);

                    int directionalLightIndex = 0;
                    Texture2DArray directionalShadowMap = nullptr;
                    for (auto [lightLocalToWorld, light, lightActive, caster] : directionalLights)
                    {
                        if (!lightActive.active)
                        {
                            continue;
                        }

                        auto& perLight = perScene.directionalLights[perScene.numDirectionalLights++];
                        perLight.direction = glm::normalize(lightLocalToWorld.mat * glm::vec4(0.0F, 0.0F, 1.0F, 0.0F));
                        perLight.color = glm::vec4(light.color, 1.0F);
                        perLight.intensity = light.intensity;

                        if (caster.contains())
                        {
                            float maxDistance =
                                caster.value().maxDistance == 0 ? camera.zFar : caster.value().maxDistance;
                            float nearDistance =
                                caster.value().nearDistance == 0 ? camera.zNear : caster.value().nearDistance;
                            int numCascades = static_cast<int>(caster.value().getCurrentSplitDistances().size()) + 1;
                            for (int i = 0; i < numCascades; i++)
                            {
                                float near = glm::mix(nearDistance, maxDistance,
                                                      i == 0 ? 0.0F
                                                             : caster.value().getCurrentSplitDistances().at(
                                                                   static_cast<unsigned long>(i) - 1));
                                float far = glm::mix(
                                    nearDistance, maxDistance,
                                    i == numCascades - 1
                                        ? 1.0F
                                        : caster.value().getCurrentSplitDistances().at(static_cast<unsigned long>(i)));

                                auto cameraView = glm::inverse(localToWorld.mat);

                                std::vector<glm::vec4> cameraFrustumCorners;
                                core::geom::getCameraFrustumCorners(cameraView, camera.projection, near, far,
                                                                    cameraFrustumCorners);

                                glm::vec3 center = {0.0F, 0.0F, 0.0F};
                                for (const auto& corner : cameraFrustumCorners)
                                {
                                    center += glm::vec3(corner);
                                }
                                center /= cameraFrustumCorners.size();

                                auto lightDir = glm::vec3(lightLocalToWorld.mat[2]);
                                auto view = glm::lookAt(center - lightDir, center, glm::vec3(0.0F, 1.0F, 0.0F));

                                // Transform frustum corners to light view space
                                for (auto& corner : cameraFrustumCorners)
                                {
                                    corner = view * corner;
                                }
                                // Find minimum/maximum coordinates along each axis
                                float minX = std::numeric_limits<float>::max();
                                float maxX = std::numeric_limits<float>::lowest();
                                float minY = std::numeric_limits<float>::max();
                                float maxY = std::numeric_limits<float>::lowest();
                                float minZ = std::numeric_limits<float>::max();
                                float maxZ = std::numeric_limits<float>::lowest();
                                for (const auto& corner : cameraFrustumCorners)
                                {
                                    minX = std::min(minX, corner.x);
                                    maxX = std::max(maxX, corner.x);
                                    minY = std::min(minY, corner.y);
                                    maxY = std::max(maxY, corner.y);
                                    minZ = std::min(minZ, corner.z);
                                    maxZ = std::max(maxZ, corner.z);
                                }

                                // Expand space between Z planes, so that objects outside the frustum can cast shadows
                                minZ -= std::abs(minZ) * 0.5F;
                                maxZ += std::abs(maxZ) * 0.5F;
                                auto proj = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);
                                perLight.matrices[i] = proj * view;
                                perLight.shadowFarSplitDistances[i / 4][i % 4] = far;
                            }

                            perLight.shadowBias = caster.value().baseSettings.bias;
                            perLight.shadowBlurRadius = caster.value().baseSettings.blurRadius;
                            perLight.numCascades = numCascades;
                            perScene.directionalLightWithShadowsId = directionalLightIndex;
                            directionalShadowMap = caster.value().shadowMaps.at(cameraEntity)->cascades;
                        }
                        directionalLightIndex++;
                    }

                    for (auto [lightLocalToWorld, light, lightActive] : pointLights)
                    {
                        if (!lightActive.active)
                        {
                            continue;
                        }

                        auto& perLight = perScene.pointLights[perScene.numPointLights++];
                        perLight.position = lightLocalToWorld.mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        perLight.color = glm::vec4(light.color, 1.0F);
                        perLight.intensity = light.intensity;
                        perLight.range = light.range;
                    }

                    for (auto [lightLocalToWorld, light, lightActive, caster] : spotLights)
                    {
                        if (!lightActive.active)
                        {
                            continue;
                        }

                        auto& perLight = perScene.spotLights[perScene.numSpotLights++];
                        perLight.position = lightLocalToWorld.mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        perLight.direction = glm::normalize(lightLocalToWorld.mat * glm::vec4(0.0F, 0.0F, 1.0F, 0.0F));
                        perLight.color = glm::vec4(light.color, 1.0F);
                        perLight.intensity = light.intensity;
                        perLight.range = light.range;
                        perLight.spotCutoff = glm::cos(glm::radians(light.spotAngle / 2.0F));
                        perLight.innerSpotCutoff = glm::cos(glm::radians(light.innerSpotAngle / 2.0F));

                        if (caster.contains())
                        {
                            // Get light viewport
                            auto slot = shadowAtlas.slotsMap.at(caster.value().baseSettings.id);

                            // The light is actually facing the direction opposite to what's visible, so rotate it.
                            auto lightView = glm::inverse(glm::scale(
                                glm::rotate(lightLocalToWorld.mat, glm::radians(180.0F), glm::vec3(0.0F, 1.0F, 0.0F)),
                                glm::vec3(1.0F / lightLocalToWorld.worldScale())));
                            auto lightProj = glm::perspective(glm::radians(light.spotAngle),
                                                              (float(shadowAtlas.getSize().x) * slot->size.x) /
                                                                  (float(shadowAtlas.getSize().y) * slot->size.y),
                                                              0.1F, light.range);
                            perLight.matrix = lightProj * lightView;
                            perLight.shadowMapOffset = slot->offset;
                            perLight.shadowMapSize = slot->size;
                            perLight.shadowBias = caster.value().baseSettings.bias;
                            perLight.shadowBlurRadius = caster.value().baseSettings.blurRadius;
                        }
                        else
                        {
                            perLight.shadowMapSize = glm::vec2(0.0F, 0.0F);
                        }
                    }

                    state.perSceneCB->fill(&perScene, sizeof(PerScene));

                    // Draw the screen quad with the GBuffer textures.
                    rd.setFramebuffer(deferredShading.frontFramebuffer);
                    rd.setViewport(static_cast<int>(drawsTo.viewportOffset.x * float(gBuffer.size.x)),
                                   static_cast<int>(drawsTo.viewportOffset.y * float(gBuffer.size.y)),
                                   static_cast<int>(drawsTo.viewportSize.x * float(gBuffer.size.x)),
                                   static_cast<int>(drawsTo.viewportSize.y * float(gBuffer.size.y)));
                    rd.setScissor(static_cast<int>(drawsTo.viewportOffset.x * float(gBuffer.size.x)),
                                  static_cast<int>(drawsTo.viewportOffset.y * float(gBuffer.size.y)),
                                  static_cast<int>(drawsTo.viewportSize.x * float(gBuffer.size.x)),
                                  static_cast<int>(drawsTo.viewportSize.y * float(gBuffer.size.y)));
                    rd.setRasterState(nullptr);
                    rd.setBlendState(nullptr);
                    rd.setDepthStencilState(nullptr);
                    rd.setShaderPipeline(state.pipeline);
                    state.positionBP->bind(gBuffer.position);
                    state.normalBP->bind(gBuffer.normal);
                    state.albedoBP->bind(gBuffer.albedo);
                    state.ssaoBP->bind(ssao.blurTexture);
                    state.shadowAtlasBP->bind(shadowAtlas.atlas);
                    // directionalShadowMap needs to be bound even if it's null, or else errors may occur on some GPUs
                    state.directionalShadowMapBP->bind(directionalShadowMap);
                    state.directionalShadowMapBP->bind(state.directionalShadowSampler);
                    state.perSceneBP->bind(state.perSceneCB);
                    state.viewportOffsetBP->setConstant(drawsTo.viewportOffset);
                    state.viewportSizeBP->setConstant(drawsTo.viewportSize);
                    rd.setVertexArray(state.screenQuad);
                    rd.drawTriangles(0, 6);
                }
            }
        });
}
