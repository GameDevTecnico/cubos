#include <vector>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "renderer.hpp"

using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using cubos::core::io::Window;

using cubos::engine::DeltaTime;
using cubos::engine::Gizmos;
using cubos::engine::GizmosRenderer;

static void drawGizmosSystem(Write<Gizmos> gizmos, Write<GizmosRenderer> gizmosRenderer, Read<DeltaTime> deltaTime)
{
    std::vector<std::size_t> toRemove;
    for (std::size_t i = 0; i < gizmos->gizmos.size(); i++)
    {
        auto& gizmo = *(gizmos->gizmos[i]);
        gizmo.draw(*gizmosRenderer);
        if (gizmo.decreaseLifespan(deltaTime->value))
        {
            toRemove.push_back(i);
        }
    }
    while (!toRemove.empty())
    {
        std::size_t i = toRemove.back();
        toRemove.pop_back();
        gizmos->gizmos[i] = gizmos->gizmos.back();
        gizmos->gizmos.pop_back();
    }
}

static void initGizmosSystem(Write<GizmosRenderer> gizmosRenderer, Read<Window> window)
{
    gizmosRenderer->init(&(*window)->renderDevice());
}

void cubos::engine::gizmosPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.addResource<Gizmos>();
    cubos.addResource<GizmosRenderer>();

    cubos.startupSystem(initGizmosSystem).tagged("cubos.gizmos.init").after("cubos.window.init");
    cubos.system(drawGizmosSystem)
        .tagged("cubos.gizmos.draw")
        .after("cubos.renderer.draw")
        .before("cubos.window.render");
}
