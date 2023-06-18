#include <imgui.h>

#include <cubos/core/settings.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/tools/asset_explorer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Write;

static void mockCamera(Write<ActiveCameras> camera, Commands cmds)
{
    camera->entities[0] = cmds.create()
                              .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
                              .add(LocalToWorld{})
                              .add(Position{{5.0F, 5.0F, -10.0F}})
                              .add(Rotation{glm::quatLookAt(glm::vec3{0.0F, 0.0F, 1.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
                              .entity();
}

static void mockSettings(Write<cubos::core::Settings> settings)
{
    settings->setString("assets.io.path", TESSERATO_ASSETS_FOLDER);
}

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);
    cubos.addPlugin(imguiPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(tools::assetExplorerPlugin);

    cubos.startupSystem(mockCamera).tagged("setup");
    cubos.startupSystem(mockSettings).tagged("setup");

    cubos.run();
}
