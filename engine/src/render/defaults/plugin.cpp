#include <cubos/engine/render/bloom/plugin.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/defaults/target.hpp>
#include <cubos/engine/render/deferred_shading/plugin.hpp>
#include <cubos/engine/render/depth/plugin.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/g_buffer_rasterizer/plugin.hpp>
#include <cubos/engine/render/hdr/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/mesh/plugin.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/shadows/atlas/plugin.hpp>
#include <cubos/engine/render/shadows/atlas_rasterizer/plugin.hpp>
#include <cubos/engine/render/shadows/cascaded/plugin.hpp>
#include <cubos/engine/render/shadows/cascaded_rasterizer/plugin.hpp>
#include <cubos/engine/render/shadows/casters/plugin.hpp>
#include <cubos/engine/render/split_screen/plugin.hpp>
#include <cubos/engine/render/ssao/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>

void cubos::engine::renderDefaultsPlugin(Cubos& cubos)
{
    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    cubos.plugin(renderTargetPlugin);
    cubos.plugin(renderVoxelsPlugin);
    cubos.plugin(lightsPlugin);
    cubos.plugin(cameraPlugin);
    cubos.plugin(shaderPlugin);
    cubos.plugin(shadowCastersPlugin);

    cubos.plugin(hdrPlugin);
    cubos.plugin(gBufferPlugin);
    cubos.plugin(renderPickerPlugin);
    cubos.plugin(renderDepthPlugin);
    cubos.plugin(renderMeshPlugin);
    cubos.plugin(splitScreenPlugin);
    cubos.plugin(shadowAtlasPlugin);
    cubos.plugin(cascadedShadowMapsPlugin);

    cubos.plugin(gBufferRasterizerPlugin);
    cubos.plugin(ssaoPlugin);
    cubos.plugin(toneMappingPlugin);
    cubos.plugin(bloomPlugin);
    cubos.plugin(shadowAtlasRasterizerPlugin);
    cubos.plugin(cascadedShadowMapsRasterizerPlugin);

    cubos.plugin(deferredShadingPlugin);

    cubos.component<RenderTargetDefaults>();

    cubos.tag(bloomTag).after(deferredShadingTag);

    cubos.observer("unpack RenderTargetDefaults components")
        .onAdd<RenderTargetDefaults>()
        .call([](Commands cmds, Query<Entity, const RenderTargetDefaults&> query) {
            for (auto [entity, defaults] : query)
            {
                cmds.remove<RenderTargetDefaults>(entity)
                    .add(entity, defaults.target)
                    .add(entity, defaults.hdr)
                    .add(entity, defaults.gBuffer)
                    .add(entity, defaults.picker)
                    .add(entity, defaults.depth)
                    .add(entity, defaults.gBufferRasterizer)
                    .add(entity, defaults.ssao)
                    .add(entity, defaults.toneMapping)
                    .add(entity, defaults.fxaa)
                    .add(entity, defaults.deferredShading);

                if (defaults.splitScreen)
                {
                    cmds.add(entity, defaults.splitScreen.value());
                }

                if (defaults.bloom)
                {
                    cmds.add(entity, defaults.bloom.value());
                }
            }
        });
}
