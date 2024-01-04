#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <tesseratos/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Write;

using cubos::engine::ActiveCameras;
using cubos::engine::Camera;
using cubos::engine::Cubos;
using cubos::engine::LocalToWorld;
using cubos::engine::Position;
using cubos::engine::Rotation;
using cubos::engine::Settings;

static void settingsSystem(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    settings->setBool("assets.io.readOnly", false);
}

static void setCameraSystem(Write<ActiveCameras> camera, Commands cmds)
{
    camera->entities[0] =
        cmds.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 1000.0F})
            .add(LocalToWorld{})
            .add(Position{{10.0F, 10.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::vec3{-1.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
    cmds.create().add(LocalToWorld{}).add(Position{{3.0F, 3.0F, 0.0F}}).entity();
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(tesseratos::plugin);
    cubos.startupSystem(settingsSystem).tagged("cubos.settings");
    cubos.startupSystem(setCameraSystem);
    cubos.run();
}
