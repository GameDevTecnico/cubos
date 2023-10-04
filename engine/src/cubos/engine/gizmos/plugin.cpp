#include <vector>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using cubos::core::io::Window;

using cubos::engine::DeltaTime;
using cubos::engine::Gizmos;

static void drawGizmosSystem(Write<Gizmos> gizmos, Read<DeltaTime> deltaTime)
{
    gizmos->drawQueuedGizmos(*deltaTime);
}

static void initGizmosSystem(Write<Gizmos> gizmos, Read<Window> window)
{
    gizmos->init((*window)->renderDevice());
}

void cubos::engine::gizmosPlugin(Cubos& cubos)
{
    cubos.addResource<Gizmos>();

    cubos.addPlugin(cubos::engine::windowPlugin);
    cubos.startupSystem(initGizmosSystem).tagged("cubos.gizmos.init").after("cubos.window.init");
    cubos.system(drawGizmosSystem)
        .tagged("cubos.gizmos.draw")
        .after("cubos.renderer.draw")
        .before("cubos.window.render");
}
