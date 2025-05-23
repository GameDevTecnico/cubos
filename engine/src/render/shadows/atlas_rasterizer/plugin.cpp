#include <cubos/core/geom/utils.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/lights/point.hpp>
#include <cubos/engine/render/lights/spot.hpp>
#include <cubos/engine/render/mesh/mesh.hpp>
#include <cubos/engine/render/mesh/plugin.hpp>
#include <cubos/engine/render/mesh/pool.hpp>
#include <cubos/engine/render/mesh/vertex.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/shadows/atlas/plugin.hpp>
#include <cubos/engine/render/shadows/atlas/point_atlas.hpp>
#include <cubos/engine/render/shadows/atlas/spot_atlas.hpp>
#include <cubos/engine/render/shadows/atlas_rasterizer/atlas_rasterizer.hpp>
#include <cubos/engine/render/shadows/atlas_rasterizer/plugin.hpp>
#include <cubos/engine/render/shadows/casters/plugin.hpp>
#include <cubos/engine/render/shadows/casters/point_caster.hpp>
#include <cubos/engine/render/shadows/casters/spot_caster.hpp>
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
        glm::mat4 lightViewProj;
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

void cubos::engine::shadowAtlasRasterizerPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("46e8da9e-5fe2-486b-85e2-f565c35eaf5e");
    static const Asset<Shader> PixelShader = AnyAsset("efe81cc1-4665-4d30-a7a6-ca5ccaa64aef");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(renderMeshPlugin);
    cubos.depends(lightsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(shadowCastersPlugin);
    cubos.depends(shadowAtlasPlugin);

    cubos.uninitResource<State>();

    cubos.resource<ShadowAtlasRasterizer>();

    cubos.startupSystem("setup Shadow Atlas Rasterizer")
        .tagged(assetsTag)
        .after(windowInitTag)
        .after(renderMeshPoolInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets, const RenderMeshPool& pool) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps), pool.vertexBuffer());
        });

    cubos.system("rasterize to shadow atlases")
        .tagged(drawToShadowAtlasTag)
        .call([](State& state, const Window& window, const RenderMeshPool& pool, SpotShadowAtlas& spotAtlas,
                 PointShadowAtlas& pointAtlas, ShadowAtlasRasterizer& rasterizer,
                 Query<const SpotShadowCaster&, const SpotLight&, const LocalToWorld&> spotLights,
                 Query<const PointShadowCaster&, const PointLight&, const LocalToWorld&> pointLights,
                 Query<Entity, const LocalToWorld&, const RenderMesh&, const RenderVoxelGrid&> meshes) {
            auto& rd = window->renderDevice();

            // Check if we need to recreate the framebuffers.
            if (rasterizer.spotAtlas != spotAtlas.atlas)
            {
                // Store textures so we can check if they change in the next frame.
                rasterizer.spotAtlas = spotAtlas.atlas;

                // Create the framebuffer.
                FramebufferDesc desc{};
                desc.targetCount = 0;
                desc.depthStencil.setTexture2DTarget(spotAtlas.atlas);
                rasterizer.spotAtlasFramebuffer = rd.createFramebuffer(desc);

                CUBOS_INFO("Recreated ShadowAtlasRasterizer's spot atlas framebuffer");
            }
            if (rasterizer.pointAtlas != pointAtlas.atlas)
            {
                // Store textures so we can check if they change in the next frame.
                rasterizer.pointAtlas = pointAtlas.atlas;

                // Create the framebuffer.
                FramebufferDesc cubeDesc{};
                cubeDesc.targetCount = 0;
                for (uint32_t i = 0; i < 6; ++i)
                {
                    cubeDesc.depthStencil.setTexture2DArrayTarget(pointAtlas.atlas, i);
                    rasterizer.pointAtlasFramebuffer[i] = rd.createFramebuffer(cubeDesc);
                }

                CUBOS_INFO("Recreated ShadowAtlasRasterizer's point atlas framebuffer");
            }

            // Bind the shader pipeline, buffers vertex array, which are common to the next passes.
            rd.setShaderPipeline(state.pipeline);
            state.perSceneBP->bind(state.perSceneCB);
            state.perMeshBP->bind(state.perMeshCB);
            rd.setVertexArray(state.vertexArray);

            // Bind the spot atlas framebuffer and set the viewport.
            rd.setFramebuffer(rasterizer.spotAtlasFramebuffer);
            rd.setViewport(0, 0, static_cast<int>(spotAtlas.getSize().x), static_cast<int>(spotAtlas.getSize().y));

            // Set the raster and depth-stencil states.
            rd.setRasterState(state.rasterState);
            rd.setBlendState(nullptr);
            rd.setDepthStencilState(state.depthStencilState);

            // Clear spot atlas
            if (!spotAtlas.cleared)
            {
                rd.clearDepth(1.0F);
                spotAtlas.cleared = true;
            }

            for (auto [caster, light, localToWorld] : spotLights)
            {
                // Get light viewport
                auto slot = spotAtlas.slotsMap.at(caster.baseSettings.id);

                // Send the PerScene data to the GPU.
                // The light is actually facing the direction opposite to what's visible, so rotate it.
                auto view = glm::inverse(
                    glm::scale(glm::rotate(localToWorld.mat, glm::radians(180.0F), glm::vec3(0.0F, 1.0F, 0.0F)),
                               glm::vec3(1.0F / localToWorld.worldScale())));
                auto proj = glm::perspective(glm::radians(light.spotAngle),
                                             (float(spotAtlas.getSize().x) * slot->size.x) /
                                                 (float(spotAtlas.getSize().y) * slot->size.y),
                                             0.1F, light.range);
                PerScene perScene{.lightViewProj = proj * view};
                state.perSceneCB->fill(&perScene, sizeof(perScene));

                // Set the viewport.
                rd.setViewport(static_cast<int>(slot->offset.x * float(spotAtlas.getSize().x)),
                               static_cast<int>(slot->offset.y * float(spotAtlas.getSize().y)),
                               static_cast<int>(slot->size.x * float(spotAtlas.getSize().x)),
                               static_cast<int>(slot->size.y * float(spotAtlas.getSize().y)));
                rd.setScissor(static_cast<int>(slot->offset.x * float(spotAtlas.getSize().x)),
                              static_cast<int>(slot->offset.y * float(spotAtlas.getSize().y)),
                              static_cast<int>(slot->size.x * float(spotAtlas.getSize().x)),
                              static_cast<int>(slot->size.y * float(spotAtlas.getSize().y)));

                // Iterate over all mesh buckets and issue draw calls.
                for (auto [meshEnt, meshLocalToWorld, mesh, grid] : meshes)
                {
                    // Send the PerMesh data to the GPU.
                    PerMesh perMesh{
                        .model = meshLocalToWorld.mat * glm::translate(glm::mat4(1.0F), grid.offset + mesh.baseOffset),
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

            // For each face of the point shadow atlas
            for (uint32_t i = 0; i < 6; ++i)
            {
                // Bind the point atlas framebuffer and set the viewport.
                rd.setFramebuffer(rasterizer.pointAtlasFramebuffer[i]);
                rd.setViewport(0, 0, static_cast<int>(pointAtlas.getSize().x),
                               static_cast<int>(pointAtlas.getSize().y));

                // Clear point atlas
                if (!pointAtlas.cleared[i])
                {
                    rd.clearDepth(1.0F);
                    pointAtlas.cleared[i] = true;
                }

                for (auto [caster, light, localToWorld] : pointLights)
                {
                    // Get light viewport
                    auto slot = pointAtlas.slotsMap.at(caster.baseSettings.id);

                    // Set the viewport.
                    rd.setViewport(static_cast<int>(slot->offset.x * float(pointAtlas.getSize().x)),
                                   static_cast<int>(slot->offset.y * float(pointAtlas.getSize().y)),
                                   static_cast<int>(slot->size.x * float(pointAtlas.getSize().x)),
                                   static_cast<int>(slot->size.y * float(pointAtlas.getSize().y)));
                    rd.setScissor(static_cast<int>(slot->offset.x * float(pointAtlas.getSize().x)),
                                  static_cast<int>(slot->offset.y * float(pointAtlas.getSize().y)),
                                  static_cast<int>(slot->size.x * float(pointAtlas.getSize().x)),
                                  static_cast<int>(slot->size.y * float(pointAtlas.getSize().y)));

                    // Send the PerScene data to the GPU.
                    PerScene perScene;
                    auto proj = glm::perspective(glm::radians(90.0F),
                                                 (float(pointAtlas.getSize().x) * slot->size.x) /
                                                     (float(pointAtlas.getSize().y) * slot->size.y),
                                                 0.1F, light.range);

                    std::vector<glm::mat4> viewMatrices;
                    core::geom::getCubeViewMatrices(
                        glm::scale(localToWorld.mat, glm::vec3(1.0F / localToWorld.worldScale())), viewMatrices);

                    perScene.lightViewProj = proj * viewMatrices[i];
                    state.perSceneCB->fill(&perScene, sizeof(perScene));

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
