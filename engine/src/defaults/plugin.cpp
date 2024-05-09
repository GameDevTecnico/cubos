#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/color_rect/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

void cubos::engine::defaultsPlugin(Cubos& cubos)
{
    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    cubos.plugin(settingsPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(fixedStepPlugin);

    cubos.plugin(assetsPlugin);
    cubos.plugin(collisionsPlugin);
    cubos.plugin(windowPlugin);

    cubos.plugin(scenePlugin);
    cubos.plugin(voxelsPlugin);
    cubos.plugin(inputPlugin);
    cubos.plugin(physicsPlugin);

    cubos.plugin(solverPlugin);
    cubos.plugin(renderDefaultsPlugin);

    cubos.plugin(canvasPlugin);
    cubos.plugin(colorRectPlugin);

    cubos.plugin(gizmosPlugin);
    cubos.plugin(imguiPlugin);

    cubos.tag(uiDrawTag).after(toneMappingTag);
    cubos.tag(gizmosDrawTag).after(toneMappingTag);
    cubos.tag(imguiEndTag).after(gizmosDrawTag);
}
