#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

using cubos::core::ecs::Write;
using namespace cubos::engine;

static void bridges(Write<Assets> assets)
{
    // Add the bridges to load .grd and .pal files.
}

void cubos::engine::voxelsPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem(bridges).tagged("cubos.assets.bridge");
}
