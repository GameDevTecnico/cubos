#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/uuid.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective_camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/depth/depth.hpp>
#include <cubos/engine/render/depth/plugin.hpp>
#include <cubos/engine/render/g_buffer/g_buffer.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/g_buffer_rasterizer/g_buffer_rasterizer.hpp>
#include <cubos/engine/render/g_buffer_rasterizer/plugin.hpp>
#include <cubos/engine/render/mesh/mesh.hpp>
#include <cubos/engine/render/mesh/plugin.hpp>
#include <cubos/engine/render/mesh/pool.hpp>
#include <cubos/engine/render/picker/picker.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core::gl;
using cubos::core::io::Window;
using cubos::engine::Asset;
using cubos::engine::RenderMeshVertex;
using cubos::engine::VoxelPalette;

CUBOS_DEFINE_TAG(cubos::engine::rasterizeToGBufferTag);

namespace
{
    // Holds the data sent per scene to the GPU.
    struct PerScene
    {
        glm::mat4 viewProj;
    };

    // Holds the data sent per mesh to the GPU.
    struct PerMesh
    {
        glm::mat4 model;
        glm::uint32_t picker;
    };

    struct State
    {
        ShaderPipeline pipeline;
        ShaderBindingPoint perSceneBP;
        ShaderBindingPoint perMeshBP;
        ShaderBindingPoint paletteBP;

        RasterState rasterState;
        DepthStencilState depthStencilState;

        VertexArray vertexArray;

        ConstantBuffer perSceneCB;
        ConstantBuffer perMeshCB;

        Texture2D paletteTexture;
        Asset<VoxelPalette> paletteAsset{};

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline, VertexBuffer vertexBuffer)
            : pipeline(pipeline)
        {
            perSceneBP = pipeline->getBindingPoint("PerScene");
            perMeshBP = pipeline->getBindingPoint("PerMesh");
            paletteBP = pipeline->getBindingPoint("palette");
            CUBOS_ASSERT(perSceneBP && perMeshBP && paletteBP,
                         "PerScene, PerMesh and palette binding points must exist");

            rasterState = renderDevice.createRasterState({
                .cullEnabled = true,
                .cullFace = Face::Back,
                .frontFace = Winding::CCW,
            });

            depthStencilState = renderDevice.createDepthStencilState({
                .depth = {.enabled = true, .writeEnabled = true},
            });

            VertexArrayDesc desc{};
            CUBOS_ASSERT(RenderMeshVertex::addVertexElements("position", "normal", "material", 0, desc));
            desc.buffers[0] = cubos::core::memory::move(vertexBuffer);
            desc.shaderPipeline = pipeline;
            vertexArray = renderDevice.createVertexArray(desc);

            perSceneCB = renderDevice.createConstantBuffer(sizeof(PerScene), nullptr, Usage::Dynamic);
            perMeshCB = renderDevice.createConstantBuffer(sizeof(PerMesh), nullptr, Usage::Dynamic);

            paletteTexture = renderDevice.createTexture2D({
                .width = 256,
                .height = 256,
                .usage = Usage::Default,
                .format = TextureFormat::RGBA32Float,
            });
        }
    };
} // namespace

void cubos::engine::gBufferRasterizerPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("ae55f7c5-c2a1-432e-b0de-386079517565");
    static const Asset<Shader> PixelShader = AnyAsset("e9a5dabc-6fc5-42ad-9cf8-f76bac49554e");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(gBufferPlugin);
    cubos.depends(renderDepthPlugin);
    cubos.depends(renderPickerPlugin);
    cubos.depends(renderMeshPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(cameraPlugin);

    cubos.tag(rasterizeToGBufferTag)
        .tagged(drawToGBufferTag)
        .tagged(drawToRenderPickerTag)
        .tagged(drawToRenderDepthTag);

    cubos.uninitResource<State>();

    cubos.component<GBufferRasterizer>();

    cubos.startupSystem("setup GBuffer Rasterizer")
        .tagged(assetsTag)
        .after(windowInitTag)
        .after(renderMeshPoolInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets, const RenderMeshPool& pool) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps), pool.vertexBuffer());
        });

    cubos.system("rasterize to GBuffer")
        .tagged(rasterizeToGBufferTag)
        .with<PerspectiveCamera>()
        .related<DrawsTo>()
        .call([](State& state, const Window& window, const RenderMeshPool& pool, const RenderPalette& palette,
                 Assets& assets, Query<const LocalToWorld&, PerspectiveCamera&> perspectiveCameras,
                 Query<Entity, GBufferRasterizer&, GBuffer&, RenderDepth&, RenderPicker&> targets,
                 Query<Entity, const LocalToWorld&, const RenderMesh&, const RenderVoxelGrid&> meshes) {
            auto& rd = window->renderDevice();

            // Update the palette's data if necessary (if the asset handle changed or the asset itself was modified).
            if (state.paletteAsset.getId() != palette.asset.getId() ||
                (!state.paletteAsset.isNull() && assets.update(state.paletteAsset)))
            {
                state.paletteAsset = assets.load(palette.asset);

                // Fetch the colors from the palette - magenta is used for non-existent materials in order to easily
                // identify errors.
                std::vector<glm::vec4> data(65536, {1.0F, 0.0F, 1.0F, 1.0F});
                assets.update(state.paletteAsset); // Make sure we store the latest asset version.
                auto voxelPalette = assets.read(state.paletteAsset);
                for (std::size_t i = 0; i < voxelPalette->size(); ++i)
                {
                    if (voxelPalette->data()[i].similarity(VoxelMaterial::Empty) < 1.0F)
                    {
                        data[i + 1] = voxelPalette->data()[i].color;
                    }
                }

                // Send the data to the GPU.
                state.paletteTexture->update(0, 0, 256, 256, data.data());

                CUBOS_INFO("Updated GBufferRasterizer's palette texture to asset {}", state.paletteAsset.getId());
            }

            for (auto [ent, rasterizer, gBuffer, depth, picker] : targets)
            {
                // Check if we need to recreate the framebuffer.
                if (rasterizer.position != gBuffer.position || rasterizer.normal != gBuffer.normal ||
                    rasterizer.albedo != gBuffer.albedo || rasterizer.frontPicker != picker.frontTexture ||
                    rasterizer.depth != depth.texture)
                {
                    // Store textures so we can check if they change in the next frame.
                    rasterizer.position = gBuffer.position;
                    rasterizer.normal = gBuffer.normal;
                    rasterizer.albedo = gBuffer.albedo;
                    rasterizer.depth = depth.texture;
                    rasterizer.frontPicker = picker.frontTexture;

                    // Create the framebuffer.
                    FramebufferDesc desc{};
                    desc.targetCount = 4;
                    desc.depthStencil.setTexture2DTarget(depth.texture);
                    desc.targets[0].setTexture2DTarget(gBuffer.position);
                    desc.targets[1].setTexture2DTarget(gBuffer.normal);
                    desc.targets[2].setTexture2DTarget(gBuffer.albedo);
                    desc.targets[3].setTexture2DTarget(picker.frontTexture);
                    rasterizer.frontFramebuffer = rd.createFramebuffer(desc);

                    CUBOS_INFO("Recreated GBufferRasterizer's front framebuffer");
                }

                // Bind the framebuffer and set the viewport.
                rd.setFramebuffer(rasterizer.frontFramebuffer);
                rd.setViewport(0, 0, static_cast<int>(gBuffer.size.x), static_cast<int>(gBuffer.size.y));

                // Set the raster and depth-stencil states.
                rd.setRasterState(state.rasterState);
                rd.setBlendState(nullptr);
                rd.setDepthStencilState(state.depthStencilState);

                // Clear each target, as needed.
                if (!depth.cleared)
                {
                    rd.clearDepth(1.0F);
                    depth.cleared = true;
                }

                if (!gBuffer.cleared)
                {
                    rd.clearTargetColor(0, 0.0F, 0.0F, 0.0F, 0.0F);
                    rd.clearTargetColor(1, 0.0F, 0.0F, 0.0F, 0.0F);
                    rd.clearTargetColor(2, 0.0F, 0.0F, 0.0F, 0.0F);
                    gBuffer.cleared = true;
                }

                if (!picker.cleared)
                {
                    rd.clearTargetColor(3, UINT16_MAX, UINT16_MAX, 0, 0);
                    picker.cleared = true;
                }

                // Find the first active camera for this target.
                for (auto [cameraLocalToWorld, camera] : perspectiveCameras.pin(1, ent))
                {
                    // Skip inactive cameras.
                    if (!camera.active)
                    {
                        continue;
                    }

                    // Send the PerScene data to the GPU.
                    auto view = glm::inverse(cameraLocalToWorld.mat);
                    auto proj =
                        glm::perspective(glm::radians(camera.fovY), float(gBuffer.size.x) / float(gBuffer.size.y),
                                         camera.zNear, camera.zFar);
                    PerScene perScene{.viewProj = proj * view};
                    state.perSceneCB->fill(&perScene, sizeof(perScene));

                    // Bind the shader, vertex array and uniform buffer.
                    rd.setShaderPipeline(state.pipeline);
                    rd.setVertexArray(state.vertexArray);
                    state.perSceneBP->bind(state.perSceneCB);
                    state.perMeshBP->bind(state.perMeshCB);
                    state.paletteBP->bind(state.paletteTexture);

                    // Iterate over all mesh buckets and issue draw calls.
                    for (auto [meshEnt, meshLocalToWorld, mesh, grid] : meshes)
                    {
                        // Send the PerMesh data to the GPU.
                        PerMesh perMesh{.model = meshLocalToWorld.mat * glm::translate(glm::mat4(1.0F), grid.offset),
                                        .picker = meshEnt.index};
                        state.perMeshCB->fill(&perMesh, sizeof(perMesh));

                        // Iterate over the buckets of the mesh (it may be split over many of them).
                        for (auto bucket = mesh.firstBucketId; bucket != RenderMeshPool::BucketId::Invalid;
                             bucket = pool.next(bucket))
                        {
                            rd.drawTriangles(pool.bucketSize() * bucket.inner, pool.vertexCount(bucket));
                        }
                    }

                    // We only use the first active camera, thus, break out of the loop immediately.
                    break;
                }

                // Swap front and back framebuffers and textures.
                // If we didn't do this, we would be recreating the framebuffer every frame, due to the picker's
                // front texture changing every frame.
                std::swap(rasterizer.frontFramebuffer, rasterizer.backFramebuffer);
                std::swap(rasterizer.frontPicker, rasterizer.backPicker);
            }
        });
}
