#include <tesseratos/asset_explorer/plugin.hpp>
#include <tesseratos/debug_camera/plugin.hpp>
#include <tesseratos/entity_inspector/plugin.hpp>
#include <tesseratos/metrics_panel/plugin.hpp>
#include <tesseratos/plugin.hpp>
#include <tesseratos/scene_editor/plugin.hpp>
#include <tesseratos/settings_inspector/plugin.hpp>
#include <tesseratos/voxel_palette_editor/plugin.hpp>
#include <tesseratos/world_inspector/plugin.hpp>

void tesseratos::plugin(cubos::engine::Cubos& cubos)
{
    cubos.addPlugin(sceneEditorPlugin);
    cubos.addPlugin(entityInspectorPlugin);
    cubos.addPlugin(worldInspectorPlugin);
    cubos.addPlugin(voxelPaletteEditorPlugin);
    cubos.addPlugin(assetExplorerPlugin);
    cubos.addPlugin(debugCameraPlugin);
    cubos.addPlugin(settingsInspectorPlugin);
    cubos.addPlugin(metricsPanelPlugin);
}
