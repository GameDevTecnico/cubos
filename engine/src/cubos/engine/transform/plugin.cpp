#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static void updateTransform()
{
    // In the future, parent-child relationships will be handled here.
}

void cubos::engine::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<Transform>();

    cubos.system(updateTransform).tagged("cubos.transform.update");
}
