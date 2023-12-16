#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/splitscreen/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static void settingsSystem(Settings& settings)
{
    // We don't load assets in this sample and we don't even have an assets folder, so we should
    // disable assets IO.
    settings.setBool("assets.io.enabled", false);
}

/// [Setting the palette]
static void setPaletteSystem(Renderer& renderer)
{
    // Create a simple palette with 3 materials (red, green and blue).
    renderer->setPalette(VoxelPalette{{
        {{1, 0, 0, 1}},
        {{0, 1, 0, 1}},
        {{0, 0, 1, 1}},
    }});
}
/// [Setting the palette]

/// [Spawning a voxel grid]
static void spawnVoxelGridSystem(Commands commands, Assets& assets)
{
    // Create a 2x2x2 grid whose voxels alternate between the materials defined in the palette.
    auto gridAsset = assets.create(VoxelGrid{{2, 2, 2}, {1, 2, 3, 1, 2, 3, 1, 2}});

    // Spawn an entity with a renderable grid component and a identity transform.
    commands.create().add(RenderableGrid{gridAsset, {-1.0F, 0.0F, -1.0F}}).add(LocalToWorld{});
}
/// [Spawning a voxel grid]

/// [Spawning a point light]
static void spawnLightSystem(Commands commands)
{
    // Spawn a point light.
    commands.create()
        .add(PointLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 1.0F, .range = 10.0F})
        .add(Position{{1.0F, 3.0F, -2.0F}});
}
/// [Spawning a point light]

/// [Setting the environment]
static void setEnvironmentSystem(RendererEnvironment& env)
{
    env.ambient = {0.2F, 0.2F, 0.2F};
    env.skyGradient[0] = {0.1F, 0.2F, 0.4F};
    env.skyGradient[1] = {0.6F, 0.6F, 0.8F};
}
/// [Setting the environment]

/// [Spawn the cameras]
static void spawnCamerasSystem(Commands commands, ActiveCameras& camera)
{
    // Spawn the a camera entity for the first viewport.
    camera.entities[0] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-3.0, 1.0F, -3.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    camera.entities[1] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-3.0, 1.0F, -3.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    camera.entities[2] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-3.0, 1.0F, -3.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
}
/// [Spawn the cameras]

int main()
{
    Cubos cubos{};

    /// [Adding the plugin]
    cubos.addPlugin(splitscreenPlugin);
    cubos.addPlugin(rendererPlugin);
    /// [Adding the plugin]

    cubos.startupSystem(settingsSystem).tagged("cubos.settings");

    /// [Adding the systems]
    cubos.startupSystem(setPaletteSystem).after("cubos.renderer.init");
    cubos.startupSystem(spawnVoxelGridSystem);
    cubos.startupSystem(spawnLightSystem);
    cubos.startupSystem(setEnvironmentSystem);
    cubos.startupSystem(spawnCamerasSystem);
    /// [Adding the systems]

    cubos.run();
}
