#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/tools/asset_explorer/plugin.hpp>
#include <cubos/engine/tools/debug_camera/plugin.hpp>
#include <cubos/engine/tools/entity_inspector/plugin.hpp>
#include <cubos/engine/tools/scene_editor/plugin.hpp>
#include <cubos/engine/tools/settings_inspector/plugin.hpp>
#include <cubos/engine/tools/world_inspector/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Write;

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

static void mockSettings(Write<Settings> settings)
{
    settings->setString("assets.io.path", TESSERATOS_ASSETS_FOLDER);
}

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(tools::sceneEditorPlugin);
    cubos.addPlugin(tools::entityInspectorPlugin);
    cubos.addPlugin(tools::worldInspectorPlugin);
    cubos.addPlugin(tools::assetExplorerPlugin);
    cubos.addPlugin(tools::debugCameraPlugin);

    cubos.startupSystem(mockCamera).tagged("setup");
    cubos.startupSystem(mockSettings).tagged("setup");

    cubos.run();
}