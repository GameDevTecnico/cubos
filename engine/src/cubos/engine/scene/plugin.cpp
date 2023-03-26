#include <cubos/engine/scene/plugin.hpp>

#include <cubos/engine/assets/plugin.hpp>

using namespace cubos::engine;

void cubos::engine::scenePlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);
}
