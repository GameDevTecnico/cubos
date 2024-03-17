#include <cubos/engine/assets/bridges/binary.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/palette.hpp>
#include <cubos/engine/voxels/plugin.hpp>

void cubos::engine::voxelsPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem("setup VoxelGrid and VoxelPalette asset bridges")
        .tagged(AssetsBridgeTag)
        .call([](Assets& assets) {
            // Add the bridges to load .grd and .pal files.
            assets.registerBridge(".grd", std::make_unique<BinaryBridge<VoxelGrid>>());
            assets.registerBridge(".pal", std::make_unique<BinaryBridge<VoxelPalette>>());
        });
}
