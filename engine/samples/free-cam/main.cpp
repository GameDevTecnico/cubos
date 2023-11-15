#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/camera.hpp>

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

static void spawnCamerasSystem(Commands commands, Write<ActiveCameras> camera)
{
    // Spawn the a camera entity for the first viewport.
    camera->entities[0] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{0.0F, 0.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FreeCameraController{})
            .entity();
}

static void echoCameraPositionSystem(Query<Read<Position>> entities)
{
    for(auto [entity, position] : entities)
    {
        CUBOS_INFO("Position vector: {}", Debug(position->vec));
    }
}

static void updateAxis(Read<Input> input, Query<Write<FreeCameraController>> entities)
{
    for(auto [entity, controller] : entities)
    {
        updateVertical(input->axis("free-vertical"), controller);
        updateHorizontalX(input->axis("free-horizontal-x"), controller);
        updateHorizontalY(input->axis("free-horizontal-y"), controller);
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

    cubos.startupSystem(spawnCamerasSystem);
    cubos.system(updateAxis);
    cubos.system(echoCameraPositionSystem);


    cubos.run();
    return 0;
}