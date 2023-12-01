#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/camera.hpp>
#include <cubos/engine/renderer/point_light.hpp>  // 4

#include <cubos/engine/voxels/grid.hpp>

#include <cubos/engine/transform/plugin.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/input/input.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>

#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/utils/free_camera_controller/plugin.hpp>

#include <cubos/core/data/old/debug_serializer.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::ecs::Query;
using cubos::core::ecs::EventReader;
using cubos::core::io::WindowEvent;
using cubos::core::io::MouseMoveEvent;
using cubos::core::io::Window;

using namespace cubos::engine;
using cubos::core::data::old::Debug;


static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d784dd3");

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void init(Read<Assets> assets, Write<Input> input)
{
    auto bindings = assets->read<InputBindings>(BindingsAsset);
    input->bind(*bindings);
}

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

static void updateAxis(Read<Input> input, Query<Write<FreeController>> entities)
{
    for(auto [entity, controller] : entities)
    {
        updateFreeVertical(input->axis("free-vertical"), controller);
        updateFreeHorizontalX(input->axis("free-horizontal-x"), controller);
        updateFreeHorizontalZ(input->axis("free-horizontal-z"), controller);
    }
}


int main()
{
    Cubos cubos{};
    cubos.addPlugin(inputPlugin);
    cubos.addPlugin(settingsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(freeCameraControllerPlugin);

    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.startupSystem(init).tagged("cubos.assets");

    cubos.startupSystem(setPaletteSystem).after("cubos.renderer.init");
    cubos.startupSystem(spawnVoxelGridSystem);
    cubos.startupSystem(spawnLightSystem);
    cubos.startupSystem(spawnCamerasSystem);
    cubos.system(updateAxis);
    //cubos.system(echoCameraPositionSystem);


    cubos.run();
    return 0;
}