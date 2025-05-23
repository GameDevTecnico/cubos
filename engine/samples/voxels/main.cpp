#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/defaults/target.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

/// [Get handles to assets]
static const Asset<VoxelGrid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
/// [Get handles to assets]

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(renderDefaultsPlugin);

    /// [Adding the plugin]
    cubos.plugin(voxelsPlugin);
    /// [Adding the plugin]

    cubos.startupSystem("configure Assets").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("create a camera").call([](Commands cmds) {
        auto targetEnt = cmds.create().add(RenderTargetDefaults{}).entity();

        cmds.create()
            .relatedTo(targetEnt, DrawsTo{})
            .add(Camera{.zNear = 0.1F, .zFar = 1000.0F})
            .add(PerspectiveCamera{.fovY = 60.0F})
            .add(Position{{50.0F, 50.0F, 50.0F}})
            .add(Rotation::lookingAt({-1.0F, -1.0F, -1.0F}, glm::vec3{0.0F, 1.0F, 0.0F}));
    });

    cubos.startupSystem("create the sun").call([](Commands cmds) {
        cmds.create()
            .add(DirectionalLight{glm::vec3(1.0F), 1.0F})
            .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
    });

    /// [Set palette]
    cubos.startupSystem("set palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });
    /// [Set palette]

    /// [Spawn car system]
    cubos.startupSystem("create a car").tagged(assetsTag).call([](Commands cmds) {
        // Create the car entity
        cmds.create().add(RenderVoxelGrid{CarAsset}).add(LocalToWorld{});
    });
    /// [Spawn car system]

    cubos.run();
}
