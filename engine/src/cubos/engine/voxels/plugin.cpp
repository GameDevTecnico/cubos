#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/grid_bridge.hpp>
#include <cubos/engine/voxels/palette_bridge.hpp>
#include <cubos/engine/voxels/plugin.hpp>

using cubos::core::ecs::Write;
using namespace cubos::engine;

static void bridges(Write<Assets> assets)
{
    // Add the bridges to load .grd and .pal files.
    assets->registerBridge(".grd", std::make_unique<GridBridge>());
    assets->registerBridge(".pal", std::make_unique<PaletteBridge>());
}

void cubos::engine::voxelsPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem(bridges).tagged("cubos.assets.bridge");
}
