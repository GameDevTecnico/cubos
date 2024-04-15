#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>

void cubos::engine::renderVoxelsPlugin(Cubos& cubos)
{
    cubos.resource<RenderPalette>();

    cubos.component<LoadRenderVoxels>();
    cubos.component<RenderVoxelGrid>();

    cubos.observer("add LoadRenderVoxels on add RenderVoxelGrid")
        .onAdd<RenderVoxelGrid>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, LoadRenderVoxels{});
            }
        });
}
