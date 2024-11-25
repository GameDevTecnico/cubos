#include <cubos/core/geom/utils.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/g_buffer/g_buffer.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/mesh/mesh.hpp>
#include <cubos/engine/render/mesh/plugin.hpp>
#include <cubos/engine/render/mesh/pool.hpp>
#include <cubos/engine/render/mesh/vertex.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/shadows/cascaded/plugin.hpp>
#include <cubos/engine/render/shadows/cascaded_rasterizer/plugin.hpp>
#include <cubos/engine/render/shadows/casters/directional_caster.hpp>
#include <cubos/engine/render/shadows/casters/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core::gl;
using cubos::core::io::Window;
using cubos::engine::RenderMeshVertex;

namespace
{
    // Holds the data sent per scene to the GPU.
    struct PerScene
    {
        glm::mat4 lightViewProj[cubos::engine::DirectionalShadowCaster::MaxCascades];
        int numCascades;
        int padding[3];
    };

    // Holds the data sent per mesh to the GPU.
    struct PerMesh
    {
        glm::mat4 model;
    };

    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        ShaderPipeline pipeline;
        ShaderBindingPoint perSceneBP;
        ShaderBindingPoint perMeshBP;

        RasterState rasterState;
        DepthStencilState depthStencilState;

        VertexArray vertexArray;

        ConstantBuffer perSceneCB;
        ConstantBuffer perMeshCB;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline, VertexBuffer vertexBuffer)
            : pipeline(pipeline)
        {
            perSceneBP = pipeline->getBindingPoint("PerScene");
            perMeshBP = pipeline->getBindingPoint("PerMesh");
            CUBOS_ASSERT(perSceneBP && perMeshBP, "PerScene and PerMesh binding points must exist");

            rasterState = renderDevice.createRasterState({
                .cullEnabled = true,
                .cullFace = Face::Back,
                .frontFace = Winding::CCW,
            });

            depthStencilState = renderDevice.createDepthStencilState({
                .depth = {.enabled = true, .writeEnabled = true},
            });

            VertexArrayDesc desc{};
            desc.elements[desc.elementCount++] = {
                .name = "position",
                .type = Type::UByte,
                .size = 3,
                .buffer = {.stride = sizeof(RenderMeshVertex),
                           .offset = offsetof(RenderMeshVertex, position),
                           .index = 0},
            };
            desc.buffers[0] = cubos::core::memory::move(vertexBuffer);
            desc.shaderPipeline = pipeline;
            vertexArray = renderDevice.createVertexArray(desc);

            perSceneCB = renderDevice.createConstantBuffer(sizeof(PerScene), nullptr, Usage::Dynamic);
            perMeshCB = renderDevice.createConstantBuffer(sizeof(PerMesh), nullptr, Usage::Dynamic);
        }
    };
} // namespace

void cubos::engine::cascadedShadowMapsRasterizerPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("852fcd11-700b-454f-8a70-ef2c5ebd3bbe");
    static const Asset<Shader> GeometryShader = AnyAsset("404e782e-b959-4120-b446-05ca8f2ecdf5");
    static const Asset<Shader> PixelShader = AnyAsset("186bc625-eb3a-4a69-8c63-0eab53e39735");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(renderMeshPlugin);
    cubos.depends(lightsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(shadowCastersPlugin);
    cubos.depends(cascadedShadowMapsPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(gBufferPlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("setup cascaded shadow maps rasterizer")
        .tagged(assetsTag)
        .after(windowInitTag)
        .after(renderMeshPoolInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets, const RenderMeshPool& pool) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto gs = assets.read(GeometryShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, gs, ps), pool.vertexBuffer());
        });

    cubos.system("rasterize to cascaded shadow maps")
        .tagged(drawToCascadedShadowMapsTag)
        .after(createGBufferTag)
        .call([](State& state, const Window& window, const RenderMeshPool& pool,
                 Query<DirectionalShadowCaster&, const DirectionalLight&, const LocalToWorld&> lights,
                 Query<Entity, const LocalToWorld&, const Camera&, const DrawsTo&, const GBuffer&> cameras,
                 Query<Entity, const LocalToWorld&, const RenderMesh&, const RenderVoxelGrid&> meshes) {
            auto& rd = window->renderDevice();

            for (auto [cameraEntity, cameraLocalToWorld, camera, drawsTo, gBuffer] : cameras)
            {
                if (!camera.active)
                {
                    continue;
                }

                for (auto [caster, light, localToWorld] : lights)
                {
                    auto& shadowMap = caster.shadowMaps.at(cameraEntity);
                    // Check if we need to recreate the framebuffer.
                    if (shadowMap->previousCascades != shadowMap->cascades)
                    {
                        // Store textures so we can check if they change in the next frame.
                        shadowMap->previousCascades = shadowMap->cascades;

                        // Create the framebuffer.
                        FramebufferDesc desc{};
                        desc.targetCount = 0;
                        desc.depthStencil.setTexture2DArrayTarget(shadowMap->cascades);
                        shadowMap->framebuffer = rd.createFramebuffer(desc);
                    }

                    // Bind the framebuffer and set the viewport.
                    rd.setFramebuffer(shadowMap->framebuffer);
                    rd.setViewport(0, 0, static_cast<int>(caster.getCurrentSize().x),
                                   static_cast<int>(caster.getCurrentSize().y));

                    // Set the raster and depth-stencil states.
                    rd.setRasterState(state.rasterState);
                    rd.setBlendState(nullptr);
                    rd.setDepthStencilState(state.depthStencilState);

                    // Clear
                    rd.clearDepth(1.0F);

                    // Send the PerScene data to the GPU.
                    PerScene perScene;

                    float maxDistance = caster.maxDistance == 0 ? camera.zFar : caster.maxDistance;
                    float nearDistance = caster.nearDistance == 0 ? camera.zNear : caster.nearDistance;
                    int numCascades = static_cast<int>(caster.getCurrentSplitDistances().size()) + 1;
                    for (int i = 0; i < numCascades; i++)
                    {
                        float near = glm::mix(
                            nearDistance, maxDistance,
                            i == 0 ? 0.0F : caster.getCurrentSplitDistances().at(static_cast<unsigned long>(i) - 1));
                        float far = glm::mix(nearDistance, maxDistance,
                                             i == numCascades - 1
                                                 ? 1.0F
                                                 : caster.getCurrentSplitDistances().at(static_cast<unsigned long>(i)));

                        auto cameraView = glm::inverse(cameraLocalToWorld.mat);

                        std::vector<glm::vec4> cameraFrustumCorners;
                        core::geom::getCameraFrustumCorners(cameraView, camera.projection, near, far,
                                                            cameraFrustumCorners);

                        glm::vec3 center = {0.0F, 0.0F, 0.0F};
                        for (const auto& corner : cameraFrustumCorners)
                        {
                            center += glm::vec3(corner);
                        }
                        center /= cameraFrustumCorners.size();

                        auto lightDir = glm::vec3(localToWorld.mat[2]);
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
                        perScene.lightViewProj[i] = proj * view;
                        perScene.numCascades = numCascades;
                    }
                    state.perSceneCB->fill(&perScene, sizeof(perScene));

                    // Bind the shader, vertex array and uniform buffer.
                    rd.setShaderPipeline(state.pipeline);
                    rd.setVertexArray(state.vertexArray);
                    state.perSceneBP->bind(state.perSceneCB);
                    state.perMeshBP->bind(state.perMeshCB);

                    // Iterate over all mesh buckets and issue draw calls.
                    for (auto [meshEnt, meshLocalToWorld, mesh, grid] : meshes)
                    {
                        // Send the PerMesh data to the GPU.
                        PerMesh perMesh{
                            .model = meshLocalToWorld.mat * glm::translate(glm::mat4(1.0F), grid.offset),
                        };
                        state.perMeshCB->fill(&perMesh, sizeof(perMesh));

                        // Iterate over the buckets of the mesh (it may be split over many of them).
                        for (auto bucket = mesh.firstBucketId; bucket != RenderMeshPool::BucketId::Invalid;
                             bucket = pool.next(bucket))
                        {
                            rd.drawTriangles(pool.bucketSize() * bucket.inner, pool.vertexCount(bucket));
                        }
                    }
                }
            }
        });
}
