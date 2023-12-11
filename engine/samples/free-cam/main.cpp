#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/renderer/camera.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp> // 4
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/utils/free_camera_controller/plugin.hpp>
#include <cubos/engine/voxels/grid.hpp>

using namespace cubos::engine;

static void setPaletteSystem(Write<Renderer> renderer)
{
    // Create a palette with 1 material (Red)
    (*renderer)->setPalette(VoxelPalette{{
        {{1, 0, 0, 1}},
    }});
}

// 3
static void spawnVoxelGridSystem(Commands commands, Write<Assets> assets)
{
    // Create a 1x1x1 grid (Cube)
    auto gridAsset = assets->create(VoxelGrid{{1, 1, 1}, {1}});

    // Spawn an entity with a renderable grid component and a identity transform.
    commands.create(RenderableGrid{gridAsset, {-1.0F, 0.0F, -1.0F}}, LocalToWorld{});
}

// 4
static void spawnLightSystem(Commands commands)
{
    // Spawn a point light.
    commands.create()
        .add(PointLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 5.0F, .range = 10.0F})
        .add(Position{{1.0F, 3.0F, -2.0F}});
}

static void spawnCamerasSystem(Commands commands, Write<ActiveCameras> camera)
{
    // Spawn the a camera entity for the first viewport.
    camera->entities[0] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{0.0F, 1.0F, -6.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FreeController{})
            .entity();
}

int main()
{
    Cubos cubos{};
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(freeCameraControllerPlugin);

    cubos.startupSystem(setPaletteSystem).after("cubos.renderer.init");
    cubos.startupSystem(spawnVoxelGridSystem);
    cubos.startupSystem(spawnLightSystem);
    cubos.startupSystem(spawnCamerasSystem);

    cubos.run();
    return 0;
}