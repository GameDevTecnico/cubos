#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/grid_bridge.hpp>
#include <cubos/engine/voxels/palette_bridge.hpp>
#include <cubos/engine/voxels/plugin.hpp>

void cubos::engine::voxelsPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);

    cubos.startupSystem("setup VoxelGrid and VoxelPalette asset bridges")
        .tagged(assetsBridgeTag)
        .call([](Assets& assets) {
            // Add the bridges to load .grd and .pal files.
            assets.registerBridge(".grd", std::make_unique<GridBridge>());
            assets.registerBridge(".pal", std::make_unique<PaletteBridge>());
        });
}
