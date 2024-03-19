#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

using namespace cubos::engine;

/// [Get handles to assets]
static const Asset<VoxelGrid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
/// [Get handles to assets]

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.addPlugin(rendererPlugin);
    /// [Adding the plugin]
    cubos.addPlugin(voxelsPlugin);
    /// [Adding the plugin]

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("create a camera").call([](Commands cmds, ActiveCameras& activeCameras) {
        // Spawn the camera entity.
        activeCameras.entities[0] = cmds.create()
                                        .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 1000.0F})
                                        .add(Position{{50.0F, 50.0F, 50.0F}})
                                        .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{-1.0F, -1.0F, -1.0F}),
                                                                      glm::vec3{0.0F, 1.0F, 0.0F})})
                                        .entity();
    });

    cubos.startupSystem("create the sun").call([](Commands cmds) {
        cmds.create()
            .add(DirectionalLight{glm::vec3(1.0F), 1.0F})
            .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
    });

    /// [Load and set palette]
    cubos.startupSystem("load and set pallete")
        .after(rendererInitTag)
        .call([](const Assets& assets, Renderer& renderer) {
            // Read the palette's data and pass it to the renderer.
            auto palette = assets.read(PaletteAsset);
            renderer->setPalette(*palette);
        });
    /// [Load and set palette]

    /// [Spawn car system]
    cubos.startupSystem("create a car").tagged(assetsTag).call([](Commands cmds, const Assets& assets) {
        // Calculate the necessary offset to center the model on (0, 0, 0).
        auto car = assets.read(CarAsset);
        glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

        // Create the car entity
        cmds.create().add(RenderableGrid{CarAsset, offset}).add(LocalToWorld{});
    });
    /// [Spawn car system]

    cubos.run();
}