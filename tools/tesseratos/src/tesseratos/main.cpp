#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include "asset_explorer/plugin.hpp"
#include "collider_gizmos/plugin.hpp"
#include "console/plugin.hpp"
#include "debug_camera/plugin.hpp"
#include "ecs_statistics/plugin.hpp"
#include "entity_inspector/plugin.hpp"
#include "entity_selector/plugin.hpp"
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

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(cubos::engine::defaultsPlugin);
    cubos.plugin(cubos::engine::freeCameraPlugin);

    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    cubos.plugin(toolboxPlugin);
    cubos.plugin(entitySelectorPlugin);

    cubos.plugin(assetExplorerPlugin);
    cubos.plugin(entityInspectorPlugin);
    cubos.plugin(worldInspectorPlugin);
    cubos.plugin(debugCameraPlugin);
    cubos.plugin(settingsInspectorPlugin);
    cubos.plugin(metricsPanelPlugin);
    cubos.plugin(colliderGizmosPlugin);
    cubos.plugin(transformGizmoPlugin);
    cubos.plugin(playPausePlugin);
    cubos.plugin(ecsStatisticsPlugin);
    cubos.plugin(consolePlugin);

    cubos.plugin(sceneEditorPlugin);
    cubos.plugin(voxelPaletteEditorPlugin);

    cubos.startupSystem("configure Assets plugin").tagged(cubos::engine::settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", TESSERATOS_ASSETS_FOLDER);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(cubos::engine::assetsTag)
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
        });

    cubos.run();
}
