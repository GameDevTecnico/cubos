#include <tesseratos/asset_explorer/plugin.hpp>
#include <tesseratos/collider_gizmos/plugin.hpp>
#include <tesseratos/debug_camera/plugin.hpp>
#include <tesseratos/ecs_statistics/plugin.hpp>
#include <tesseratos/entity_inspector/plugin.hpp>
#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/metrics_panel/plugin.hpp>
#include <tesseratos/play_pause/plugin.hpp>
#include <tesseratos/plugin.hpp>
#include <tesseratos/scene_editor/plugin.hpp>
#include <tesseratos/settings_inspector/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>
#include <tesseratos/transform_gizmo/plugin.hpp>
#include <tesseratos/voxel_palette_editor/plugin.hpp>
#include <tesseratos/world_inspector/plugin.hpp>

void tesseratos::plugin(cubos::engine::Cubos& cubos)
{
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

    cubos.plugin(sceneEditorPlugin);
    cubos.plugin(voxelPaletteEditorPlugin);
}
