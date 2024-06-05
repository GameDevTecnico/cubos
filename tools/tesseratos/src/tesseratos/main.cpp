#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include "asset_explorer/plugin.hpp"
#include "collider_gizmos/plugin.hpp"
#include "console/plugin.hpp"
#include "debug_camera/plugin.hpp"
#include "ecs_statistics/plugin.hpp"
#include "entity_inspector/plugin.hpp"
#include "entity_selector/plugin.hpp"
#include "menu_bar/plugin.hpp"
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
    cubos.plugin(menuBarPlugin);

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

    cubos.startupSystem("add some menu items just for testing purposes").call([](Commands cmds) {
        auto project = cmds.create().add(MenuBarItem{"Project", 0}).entity();
        auto view = cmds.create().add(MenuBarItem{"View", 1}).entity();
        cmds.create().add(MenuBarItem{"Help", 2});

        cmds.create().add(MenuBarItem{"New", 0}).relatedTo(project, ChildOf{});
        cmds.create().add(MenuBarItem{"Open", 1}).relatedTo(project, ChildOf{});
        cmds.create().add(MenuBarItem{"Save", 2}).relatedTo(project, ChildOf{});

        cmds.create().add(MenuBarItem{"Console", 0}).relatedTo(view, ChildOf{});
        cmds.create().add(MenuBarItem{"Assets Explorer", 1}).relatedTo(view, ChildOf{});
        cmds.create().add(MenuBarItem{"Entity Inspector", 2}).relatedTo(view, ChildOf{});
        cmds.create().add(MenuBarItem{"World Inspector", 3}).relatedTo(view, ChildOf{});
    });

    cubos.run();
}
