#include <cubos/engine/cubos.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Write;

using namespace cubos::engine;

static void settingsSystem(Write<Settings> settings)
{
    // We don't load assets in this sample and we don't even have an assets folder, so we should
    // disable assets IO.
    settings->setBool("assets.io.enabled", false);
}

static void setCameraSystem(Write<ActiveCameras> camera, Commands cmds)
{
    camera->entities[0] =
        cmds.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(LocalToWorld{})
            .add(Position{{5.0F, 5.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::vec3{-1.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
    camera->entities[1] = cmds.create()
                              .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
                              .add(LocalToWorld{})
                              .add(Position{{-25.0F, 25.0F, 0.0F}})
                              .add(Rotation{glm::quatLookAt(glm::vec3{1.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
                              .entity();
    camera->entities[2] = cmds.create()
                              .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
                              .add(LocalToWorld{})
                              .add(Position{{-25.0F, -25.0F, 0.0F}})
                              .add(Rotation{glm::quatLookAt(glm::vec3{1.0F, 1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
                              .entity();
    camera->entities[3] = cmds.create()
                              .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
                              .add(LocalToWorld{})
                              .add(Position{{25.0F, -25.0F, 0.0F}})
                              .add(Rotation{glm::quatLookAt(glm::vec3{-1.0F, 1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
                              .entity();
}

/// [System]
static void drawSystem(Write<Gizmos> gizmos)
{
    /// [Lines]
    gizmos->color({1, 1, 1});
    gizmos->drawLine("separator line", {1.0F, 0.5F, 0.5F}, {0.0F, 0.5F, 0.5F}, 0, Gizmos::Space::Screen);
    gizmos->drawLine("separator line", {0.5F, 1.0F, 0.5F}, {0.5F, 0.0F, 0.5F}, 0, Gizmos::Space::Screen);
    /// [Lines]

    /// [WireBox]
    gizmos->color({1, 0.5, 1});
    gizmos->drawBox("test box", {0.4, 0.4, 0}, {0.55, 0.55, 0}, 0, Gizmos::Space::View);
    /// [WireBox]

    /// [Box]
    gizmos->color({0.5F, 1, 1});
    gizmos->drawWireBox("test box", {0.5F, 0.5F, 0.5F}, {-1, -1, -1}, 0, Gizmos::Space::World);
    /// [Box]

    ///[Cut Cone]
    gizmos->color({0.5F, 0.3F, 1});
    gizmos->drawCutCone("test cut cone", {0.7F, 0.7F, 0.7F}, 5.0F, {-3, -3, -3}, 3.0F, 0, Gizmos::Space::World);
    ///[Cut Cone]
}
/// [System]

/// [Start Up System]
static void drawStartingLineSystem(Write<Gizmos> gizmos)
{
    gizmos->color({1, 0, 0});
    gizmos->drawLine("test line", {0, 1, 0}, {1, 0, 1}, 10, Gizmos::Space::Screen);
}
/// [Start Up System]

int main(int argc, char** argv)
{
    /// [Adding plugin]
    Cubos cubos{argc, argv};
    cubos.addPlugin(gizmosPlugin);
    /// [Adding plugin]
    cubos.addPlugin(rendererPlugin);

    cubos.startupSystem(settingsSystem).tagged("cubos.settings");
    cubos.startupSystem(setCameraSystem);
    cubos.startupSystem(drawStartingLineSystem).tagged("sample.init").after("cubos.gizmos.init");

    cubos.system(drawSystem);

    cubos.run();
}
