#include <cubos/engine/tools/collider_gizmos/plugin.hpp>
#include <cubos/engine/tools/console/plugin.hpp>
#include <cubos/engine/tools/debug_camera/plugin.hpp>
#include <cubos/engine/tools/ecs_statistics/plugin.hpp>
#include <cubos/engine/tools/entity_inspector/plugin.hpp>
#include <cubos/engine/tools/metrics_panel/plugin.hpp>
#include <cubos/engine/tools/play_pause/plugin.hpp>
#include <cubos/engine/tools/plugin.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>
#include <cubos/engine/tools/settings_inspector/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>
#include <cubos/engine/tools/transform_gizmo/plugin.hpp>
#include <cubos/engine/tools/world_inspector/plugin.hpp>

void cubos::engine::toolsPlugin(Cubos& cubos)
{
    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    cubos.plugin(toolboxPlugin);
    cubos.plugin(selectionPlugin);

    cubos.plugin(entityInspectorPlugin);
    cubos.plugin(worldInspectorPlugin);
    cubos.plugin(debugCameraPlugin);
    cubos.plugin(settingsInspectorPlugin);
    cubos.plugin(metricsPanelPlugin);
    cubos.plugin(colliderGizmosPlugin);
    cubos.plugin(transformGizmoPlugin);
    cubos.plugin(playPauseToolPlugin);
    cubos.plugin(ecsStatisticsPlugin);
    cubos.plugin(consolePlugin);
}
