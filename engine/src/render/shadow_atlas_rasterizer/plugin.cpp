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
#include <cubos/engine/render/shadow_atlas/plugin.hpp>
#include <cubos/engine/render/shadow_atlas/shadow_atlas.hpp>
#include <cubos/engine/render/shadow_atlas_rasterizer/plugin.hpp>
#include <cubos/engine/render/shadow_atlas_rasterizer/shadow_atlas_rasterizer.hpp>
#include <cubos/engine/render/shadows/plugin.hpp>
#include <cubos/engine/render/shadows/point_caster.hpp>
#include <cubos/engine/render/shadows/spot_caster.hpp>
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
    struct PerSceneCube
    {
        glm::mat4 lightViewProj[6];
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
        ShaderPipeline cubePipeline;
        ShaderBindingPoint perSceneBP;
        ShaderBindingPoint perMeshBP;
        ShaderBindingPoint perSceneCubeBP;
        ShaderBindingPoint perMeshCubeBP;

        RasterState rasterState;
        DepthStencilState depthStencilState;

        VertexArray vertexArray;

        ConstantBuffer perSceneCB;
        ConstantBuffer perSceneCubeCB;
        ConstantBuffer perMeshCB;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline, const ShaderPipeline& cubePipeline,
              VertexBuffer vertexBuffer)
            : pipeline(pipeline)
            , cubePipeline(cubePipeline)
        {
            perSceneBP = pipeline->getBindingPoint("PerScene");
            perMeshBP = pipeline->getBindingPoint("PerMesh");
            perSceneCubeBP = cubePipeline->getBindingPoint("PerScene");
            perMeshCubeBP = cubePipeline->getBindingPoint("PerMesh");
            CUBOS_ASSERT(perSceneBP && perMeshBP && perSceneCubeBP && perMeshCubeBP,
                         "PerScene and PerMesh binding points must exist");

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
            perSceneCubeCB = renderDevice.createConstantBuffer(sizeof(PerSceneCube), nullptr, Usage::Dynamic);
            perMeshCB = renderDevice.createConstantBuffer(sizeof(PerMesh), nullptr, Usage::Dynamic);
        }
    };
} // namespace

void cubos::engine::shadowAtlasRasterizerPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("46e8da9e-5fe2-486b-85e2-f565c35eaf5e");
    static const Asset<Shader> PixelShader = AnyAsset("efe81cc1-4665-4d30-a7a6-ca5ccaa64aef");

    static const Asset<Shader> VertexShaderCube = AnyAsset("b9ac4697-c0d3-4e2d-9607-3da50f071d2d");
    static const Asset<Shader> GeometryShaderCube = AnyAsset("e0c5f304-fccf-496e-b181-08a3007f15b0");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(renderMeshPlugin);
    cubos.depends(lightsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(shadowsPlugin);
    cubos.depends(shadowAtlasPlugin);

    cubos.uninitResource<State>();

    cubos.resource<ShadowAtlasRasterizer>();

    cubos.startupSystem("setup ShadowAtlas Rasterizer")
        .tagged(assetsTag)
        .after(windowInitTag)
        .after(renderMeshPoolInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets, const RenderMeshPool& pool) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            auto vsCube = assets.read(VertexShaderCube)->shaderStage();
            auto gsCube = assets.read(GeometryShaderCube)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps),
                                        rd.createShaderPipeline(vsCube, gsCube, ps), pool.vertexBuffer());
        });

    cubos.system("rasterize to ShadowAtlas")
        .tagged(drawToShadowAtlasTag)
        .call([](State& state, const Window& window, const RenderMeshPool& pool, ShadowAtlas& atlas,
                 ShadowAtlasRasterizer& rasterizer,
                 Query<const SpotShadowCaster&, const SpotLight&, const LocalToWorld&> spotLights,
                 Query<const PointShadowCaster&, const PointLight&, const LocalToWorld&> pointLights,
                 Query<Entity, const LocalToWorld&, const RenderMesh&, const RenderVoxelGrid&> meshes) {
            auto& rd = window->renderDevice();

            // Check if we need to recreate the framebuffers.
            if (rasterizer.atlas != atlas.spotAtlas)
            {
                // Store textures so we can check if they change in the next frame.
                rasterizer.atlas = atlas.spotAtlas;

                // Create the framebuffers.
                FramebufferDesc desc{};
                desc.targetCount = 0;
                desc.depthStencil.setTexture2DTarget(atlas.spotAtlas);
                rasterizer.spotAtlasFramebuffer = rd.createFramebuffer(desc);

                FramebufferDesc cubeDesc{};
                cubeDesc.targetCount = 0;
                cubeDesc.depthStencil.setTexture2DArrayTarget(atlas.pointAtlas);
                rasterizer.pointAtlasFramebuffer = rd.createFramebuffer(cubeDesc);

                CUBOS_INFO("Recreated ShadowAtlasRasterizer's framebuffers");
            }

            // Bind the spot atlas framebuffer and set the viewport.
            rd.setFramebuffer(rasterizer.spotAtlasFramebuffer);
            rd.setViewport(0, 0, static_cast<int>(atlas.getSpotAtlasSize().x),
                           static_cast<int>(atlas.getSpotAtlasSize().y));

            // Set the raster and depth-stencil states.
            rd.setRasterState(state.rasterState);
            rd.setBlendState(nullptr);
            rd.setDepthStencilState(state.depthStencilState);

            // Clear spot atlas
            if (!atlas.cleared)
            {
                rd.clearDepth(1.0F);
            }

            for (auto [caster, light, localToWorld] : spotLights)
            {
                // Get light viewport
                auto slot = atlas.slotsMap.at(caster.baseSettings.id);

                // Send the PerScene data to the GPU.
                // The light is actually facing the direction opposite to what's visible, so rotate it.
                auto view = glm::inverse(
                    glm::scale(glm::rotate(localToWorld.mat, glm::radians(180.0F), glm::vec3(0.0F, 1.0F, 0.0F)),
                               glm::vec3(1.0F / localToWorld.worldScale())));
                auto proj = glm::perspective(glm::radians(light.spotAngle),
                                             (float(atlas.getSpotAtlasSize().x) * slot->size.x) /
                                                 (float(atlas.getSpotAtlasSize().y) * slot->size.y),
                                             0.1F, light.range);
                PerScene perScene{.lightViewProj = proj * view};
                state.perSceneCB->fill(&perScene, sizeof(perScene));

                // Set the viewport.
                rd.setViewport(static_cast<int>(slot->offset.x * float(atlas.getSpotAtlasSize().x)),
                               static_cast<int>(slot->offset.y * float(atlas.getSpotAtlasSize().y)),
                               static_cast<int>(slot->size.x * float(atlas.getSpotAtlasSize().x)),
                               static_cast<int>(slot->size.y * float(atlas.getSpotAtlasSize().y)));
                rd.setScissor(static_cast<int>(slot->offset.x * float(atlas.getSpotAtlasSize().x)),
                              static_cast<int>(slot->offset.y * float(atlas.getSpotAtlasSize().y)),
                              static_cast<int>(slot->size.x * float(atlas.getSpotAtlasSize().x)),
                              static_cast<int>(slot->size.y * float(atlas.getSpotAtlasSize().y)));
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

            // Bind the point atlas framebuffer and set the viewport.
            rd.setFramebuffer(rasterizer.pointAtlasFramebuffer);
            rd.setViewport(0, 0, static_cast<int>(atlas.getPointAtlasSize().x),
                           static_cast<int>(atlas.getPointAtlasSize().y));

            // Clear point atlas
            if (!atlas.cleared)
            {
                rd.clearDepth(1.0F);
                atlas.cleared = true;
            }

            for (auto [caster, light, localToWorld] : pointLights)
            {
                // Get light viewport
                auto slot = atlas.slotsMap.at(caster.baseSettings.id);

                // Set the viewport.
                rd.setViewport(static_cast<int>(slot->offset.x * float(atlas.getPointAtlasSize().x)),
                               static_cast<int>(slot->offset.y * float(atlas.getPointAtlasSize().y)),
                               static_cast<int>(slot->size.x * float(atlas.getPointAtlasSize().x)),
                               static_cast<int>(slot->size.y * float(atlas.getPointAtlasSize().y)));
                rd.setScissor(static_cast<int>(slot->offset.x * float(atlas.getPointAtlasSize().x)),
                              static_cast<int>(slot->offset.y * float(atlas.getPointAtlasSize().y)),
                              static_cast<int>(slot->size.x * float(atlas.getPointAtlasSize().x)),
                              static_cast<int>(slot->size.y * float(atlas.getPointAtlasSize().y)));

                // Send the PerScene data to the GPU.
                PerSceneCube perScene;
                auto proj = glm::perspective(glm::radians(90.0F),
                                             (float(atlas.getPointAtlasSize().x) * slot->size.x) /
                                                 (float(atlas.getPointAtlasSize().y) * slot->size.y),
                                             0.1F, light.range);

                for (int i = 0; i < 6; i++)
                {
                    glm::mat4 view;
                    switch (i)
                    {
                    case 0:
                        view = glm::inverse(glm::scale(localToWorld.mat, glm::vec3(1.0F / localToWorld.worldScale())));
                        break;
                    case 1:
                        view = glm::inverse(
                            glm::scale(glm::rotate(localToWorld.mat, glm::radians(180.0F), glm::vec3(0.0F, 1.0F, 0.0F)),
                                       glm::vec3(1.0F / localToWorld.worldScale())));
                        break;
                    case 2:
                        view = glm::inverse(
                            glm::scale(glm::rotate(localToWorld.mat, glm::radians(90.0F), glm::vec3(0.0F, 1.0F, 0.0F)),
                                       glm::vec3(1.0F / localToWorld.worldScale())));
                        break;
                    case 3:
                        view = glm::inverse(
                            glm::scale(glm::rotate(localToWorld.mat, glm::radians(-90.0F), glm::vec3(0.0F, 1.0F, 0.0F)),
                                       glm::vec3(1.0F / localToWorld.worldScale())));
                        break;
                    case 4:
                        view = glm::inverse(
                            glm::scale(glm::rotate(localToWorld.mat, glm::radians(90.0F), glm::vec3(1.0F, 0.0F, 0.0F)),
                                       glm::vec3(1.0F / localToWorld.worldScale())));
                        break;
                    case 5:
                        view = glm::inverse(
                            glm::scale(glm::rotate(localToWorld.mat, glm::radians(-90.0F), glm::vec3(1.0F, 0.0F, 0.0F)),
                                       glm::vec3(1.0F / localToWorld.worldScale())));
                        break;
                    default:
                        view = glm::inverse(glm::scale(localToWorld.mat, glm::vec3(1.0F / localToWorld.worldScale())));
                        break;
                    }

                    perScene.lightViewProj[i] = proj * view;
                }
                state.perSceneCubeCB->fill(&perScene, sizeof(perScene));

                // Bind the shader, vertex array and uniform buffer.
                rd.setShaderPipeline(state.cubePipeline);
                rd.setVertexArray(state.vertexArray);
                state.perSceneCubeBP->bind(state.perSceneCubeCB);
                state.perMeshCubeBP->bind(state.perMeshCB);

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
        });
}
