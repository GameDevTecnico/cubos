#include <cubos/engine/assets/bridges/binary.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/palette.hpp>
#include <cubos/engine/voxels/plugin.hpp>

using cubos::core::ecs::Write;
using namespace cubos::engine;

static void bridges(Write<Assets> assets)
{
    // Add the bridges to load .grd and .pal files.
    assets->registerBridge(".grd", std::make_unique<BinaryBridge<Grid>>());
    assets->registerBridge(".pal", std::make_unique<BinaryBridge<Palette>>());
}

void cubos::engine::voxelsPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem(bridges).tagged("cubos.assets.bridge");
}
