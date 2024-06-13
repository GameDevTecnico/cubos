#include <imgui.h>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include "asset_explorer/plugin.hpp"
#include "collider_gizmos/plugin.hpp"
#include "debug_camera/plugin.hpp"
#include "ecs_statistics/plugin.hpp"
#include "entity_inspector/plugin.hpp"
#include "entity_selector/plugin.hpp"
#include "game/plugin.hpp"
#include "game_imgui/plugin.hpp"
#include "game_window/plugin.hpp"
#include "metrics_panel/plugin.hpp"
#include "play_pause/plugin.hpp"
#include "scene_editor/plugin.hpp"
#include "settings_inspector/plugin.hpp"
#include "toolbox/plugin.hpp"
#include "transform_gizmo/plugin.hpp"
#include "voxel_palette_editor/plugin.hpp"
#include "world_inspector/plugin.hpp"

using namespace cubos::engine;
using namespace tesseratos;

static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842e5");
static const Asset<InputBindings> SampleBindingsAsset = AnyAsset("f93e9749-edf1-461c-b997-3f4e99c85a28");
static const Asset<Scene> SampleSceneAsset = AnyAsset("118f8436-4414-47d3-b617-293fc68c1c73");
static const Asset<VoxelPalette> SamplePaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");

static void sampleGamePlugin(Cubos& cubos)
{
    // This is just a sample 'game' plugin I added to test the game window.
    // In a future PR, this should be moved to its own CMake project, and loaded at runtime from a shared library.

    cubos.plugin(defaultsPlugin);
    cubos.plugin(cubos::engine::freeCameraPlugin);

    cubos.startupSystem("spawn sample scene")
        .tagged(assetsTag)
        .call([](Commands cmds, Assets& assets, RenderEnvironment& environment, RenderPalette& palette, Input& input) {
            palette.asset = SamplePaletteAsset;
            environment.ambient = {0.1F, 0.1F, 0.1F};
            environment.skyGradient[0] = {0.1F, 0.2F, 0.4F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            cmds.spawn(assets.read<Scene>(SampleSceneAsset)->blueprint);
            input.bind(*assets.read<InputBindings>(SampleBindingsAsset));
        });

    cubos.system("ImGui demo window").tagged(imguiTag).call([]() { ImGui::ShowDemoWindow(); });

    cubos.system("toggle FreeCameraController").call([](Input& input, Query<FreeCameraController&> controllers) {
        if (input.justPressed("toggle-camera"))
        {
            for (auto [controller] : controllers)
            {
                controller.enabled = !controller.enabled;
            }
        }
    });
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(cubos::engine::defaultsPlugin);

    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    // TODO: Go through each of the commented plugins below, convert them to use the new Game resource, and uncomment
    // them.

    cubos.plugin(gamePlugin);
    cubos.plugin(toolboxPlugin);
    //  cubos.plugin(entitySelectorPlugin);

    cubos.plugin(gameWindowPlugin);
    cubos.plugin(gameImGuiPlugin);
    cubos.plugin(assetExplorerPlugin);
    // cubos.plugin(entityInspectorPlugin);
    // cubos.plugin(worldInspectorPlugin);
    // cubos.plugin(debugCameraPlugin);
    // cubos.plugin(settingsInspectorPlugin);
    // cubos.plugin(metricsPanelPlugin);
    // cubos.plugin(colliderGizmosPlugin);
    // cubos.plugin(transformGizmoPlugin);
    // cubos.plugin(playPausePlugin);
    // cubos.plugin(ecsStatisticsPlugin);

    // cubos.plugin(sceneEditorPlugin);
    // cubos.plugin(voxelPaletteEditorPlugin);

    cubos.startupSystem("configure Assets plugin").tagged(cubos::engine::settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", TESSERATOS_ASSETS_FOLDER);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(cubos::engine::assetsTag)
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
        });

    cubos.system("load and start the game for testing purposes").call([](Game& game) {
        if (!game.isLoaded())
        {
            game.load(sampleGamePlugin);
            game.start();
        }
    });

    cubos.run();
}
