
#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

#include "components.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::gl::Grid;
using cubos::core::gl::Palette;

using namespace cubos::engine;

/// [Get handles to assets]
static const Asset<Grid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<Palette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
/// [Get handles to assets]

/// [Spawner resource]
struct Spawner
{
    float timer = 0.0F;
    int x = -1;
    int y = -1;
};
/// [Spawner resource]

/// [Set settings]
static void settingsSystem(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}
/// [Set settings]

/// [Load, add materials and set pallete]
static void setPalleteSystem(Write<Assets> assets, Write<Renderer> renderer)
{
    // Load the palette asset and add two colors to it.
    auto palette = assets->write(PaletteAsset);
    palette->add({{0.1F, 0.1F, 0.1F, 1.0F}});
    palette->add({{0.9F, 0.9F, 0.9F, 1.0F}});

    // Set the renderer's palette to the one we just modified.
    (*renderer)->setPalette(*palette);
}
/// [Load, add materials and set pallete]

/// [Create and spawn floor]
static void spawnFloorSystem(Commands cmds, Write<Assets> assets)
{
    // Get the materials previously added to the palette
    auto palette = assets->read(PaletteAsset);
    auto black = palette->find({{0.1F, 0.1F, 0.1F, 1.0F}});
    auto white = palette->find({{0.9F, 0.9F, 0.9F, 1.0F}});

    // Generate a new grid asset for the floor.
    auto floorGrid = Grid({256, 1, 256});
    for (int x = 0; x < 256; ++x)
    {
        for (int z = 0; z < 256; ++z)
        {
            floorGrid.set({x, 0, z}, (x + z) % 2 == 0 ? black : white);
        }
    }
    auto floorOffset = glm::vec3(floorGrid.size().x, 0.0F, floorGrid.size().z) / -2.0F;
    floorOffset.y = -1.0F;

    auto floorAsset = assets->create(std::move(floorGrid));

    // Spawn the floor entity.
    cmds.create()
        .add(RenderableGrid{floorAsset, floorOffset})
        .add(LocalToWorld{})
        .add(Scale{4.0F});
}
/// [Create and spawn floor]

/// [Spawn camera]
static void spawnCameraSystem(Commands cmds, Write<ActiveCameras> activeCameras)
{
    // Spawn the camera entity.
    activeCameras->entities[0] =
        cmds.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 1000.0F})
            .add(LocalToWorld{})
            .add(Position{{0.0F, 120.0F, -200.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
}
/// [Spawn camera]

/// [Spawn light]
static void spawnLightSystem(Commands cmds)
{
    // Spawn the sun.
    cmds.create()
        .add(DirectionalLight{glm::vec3(1.0F), 1.0F})
        .add(LocalToWorld{})
        .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
}
/// [Spawn light]

/// [Spawn system]
static void spawnSystem(Commands cmds, Write<Spawner> spawner, Read<DeltaTime> deltaTime, Read<Assets> assets)
{
    spawner->timer += deltaTime->value;
    if (spawner->timer >= 1.0F && spawner->y < 2)
    {
        auto car = assets->read(CarAsset);
        glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

        cmds.create()
            .add(Car{})
            .add(RenderableGrid{CarAsset, offset})
            .add(LocalToWorld{})
            .add(Position{{80.0F * static_cast<float>(spawner->x), 0.0F, 80.0F * static_cast<float>(spawner->y)}})
            .add(Rotation{});

        spawner->timer = 0;
        /// [Spawn system]
        spawner->x += 1;
        if (spawner->x == 2)
        {
            spawner->y++;
            spawner->x = -1;
        }
    }
}

/// [Move system]
static void moveSystem(Query<Write<Car>, Write<Position>, Write<Rotation>> query, Read<DeltaTime> deltaTime)
{
    for (auto [entity, car, position, rotation] : query)
    {
        car->vel = rotation->quat * glm::vec3(0.0F, 0.0F, 50.0F);
        car->angVel = 3.0F;

        position->vec += deltaTime->value * car->vel;
        rotation->quat = glm::angleAxis(deltaTime->value * car->angVel, glm::vec3(0.0F, 1.0F, 0.0F)) * rotation->quat;
    }
}
/// [Move system]

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(settingsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    /// [Adding the render and voxels plugin]
    
    /// [Adding component and resource]
    cubos.addComponent<Car>();
    cubos.addResource<Spawner>();
    /// [Adding component and resource]

    /// [Adding systems]
    cubos.startupSystem(settingsSystem).tagged("cubos.settings");
    cubos.startupSystem(setPalleteSystem).tagged("palette.set").after("cubos.renderer.init");
    cubos.startupSystem(spawnFloorSystem).after("palette.set");
    cubos.startupSystem(spawnCameraSystem);
    cubos.startupSystem(spawnLightSystem);
    cubos.system(spawnSystem);
    cubos.system(moveSystem);
    /// [Adding systems]

    cubos.run();
}