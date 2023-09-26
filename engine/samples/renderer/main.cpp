#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Write;

using namespace cubos::engine;

static void settingsSystem(Write<Settings> settings)
{
    // We don't load assets in this sample and we don't even have an assets folder, so we should
    // disable assets IO.
    settings->setBool("assets.io.enabled", false);
}

/// [Setting the palette]
static void setPaletteSystem(Write<Renderer> renderer)
{
    using cubos::core::gl::Palette;

    // Create a simple palette with 3 materials (red, green and blue).
    (*renderer)->setPalette(Palette{{
        {{1, 0, 0, 1}},
        {{0, 1, 0, 1}},
        {{0, 0, 1, 1}},
    }});
}
/// [Setting the palette]

/// [Spawning a voxel grid]
static void spawnVoxelGridSystem(Commands commands, Write<Assets> assets)
{
    using cubos::core::gl::Grid;

    // Create a 2x2x2 grid whose voxels alternate between the materials defined in the palette.
    auto gridAsset = assets->create(Grid{{2, 2, 2}, {1, 2, 3, 1, 2, 3, 1, 2}});

    // Spawn an entity with a renderable grid component and a identity transform.
    commands.create(RenderableGrid{gridAsset, {-1.0F, 0.0F, -1.0F}}, LocalToWorld{});
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
static void setEnvironmentSystem(Write<RendererEnvironment> env)
{
    env->ambient = {0.2F, 0.2F, 0.2F};
    env->skyGradient[0] = {0.1F, 0.2F, 0.4F};
    env->skyGradient[1] = {0.6F, 0.6F, 0.8F};
}
/// [Setting the environment]

/// [Spawn the cameras]
static void spawnCamerasSystem(Commands commands, Write<ActiveCameras> camera)
{
    // Spawn the a camera entity for the first viewport.
    camera->entities[0] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-3.0, 1.0F, -3.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    // Add two other viewports using the same camera, which splits the screen in three.
    camera->entities[1] = camera->entities[0];
    camera->entities[2] = camera->entities[0];
}
/// [Spawn the cameras]

int main()
{
    Cubos cubos{};

    /// [Adding the plugin]
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
