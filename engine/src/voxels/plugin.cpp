#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/grid_bridge.hpp>
#include <cubos/engine/voxels/model_bridge.hpp>
#include <cubos/engine/voxels/palette_bridge.hpp>
#include <cubos/engine/voxels/plugin.hpp>

void cubos::engine::voxelsPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);

    cubos.startupSystem("setup VoxelGrid, VoxelPalette & VoxelModel asset bridges")
        .tagged(assetsBridgeTag)
        .call([](Assets& assets) {
            // Add the bridges to load .grd, .pal and .qb files.
            assets.registerBridge(".grd", std::make_unique<VoxelGridBridge>());
            assets.registerBridge(".pal", std::make_unique<VoxelPaletteBridge>());
            assets.registerBridge(".qb", std::make_unique<VoxelModelBridge>());
        });
}
