#include <cubos/engine/cubos.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Write;

using cubos::engine::Gizmos;

using namespace cubos::engine;

static void mockCamera(Write<ActiveCameras> camera, Commands cmds)
{
    camera->entities[0] = cmds.create()
                              .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
                              .add(LocalToWorld{})
                              .add(Position{{5.0F, 5.0F, -10.0F}})
                              .add(Rotation{glm::quatLookAt(glm::vec3{0.0F, 0.0F, 1.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
                              .entity();
}

/// [System]
static void drawSystem(Write<Gizmos> gizmos)
{
    gizmos->color({0, 0, 1});
    gizmos->drawLine("test line", {0, 0, 0}, {1, 1, 1}, 0);

    gizmos->color({0, 1, 1});
    gizmos->drawBox("test box", {0.4, 0.4, 0}, {0.6, 0.6, 1}, 0);

    gizmos->color({1, 0, 1});
    gizmos->drawWireBox("test box", {0.2, 0.7, 0}, {0.3, 0.1, 0}, 0);
}
/// [System]

/// [Start Up System]
static void drawStartingLineSystem(Write<Gizmos> gizmos)
{
    gizmos->color({1, 0, 0});
    gizmos->drawLine("test line", {0, 1, 0}, {1, 0, 1}, 10);
}
/// [Start Up System]

/// [Run]
int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(gizmosPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.startupSystem(mockCamera);
    cubos.startupSystem(drawStartingLineSystem).tagged("sample.init").after("cubos.gizmos.init");
    cubos.system(drawSystem);
    cubos.run();
}
/// [Run]
