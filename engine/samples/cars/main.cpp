
#include <cubos/engine/renderer/light.hpp>
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

static const Asset<Grid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<Palette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");

struct Spawner
{
    float timer = 0.0F;
    int x = -1;
    int y = -1;
};

static void settings(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void setup(Commands cmds, Write<Assets> assets, Write<Renderer> renderer, Write<ActiveCameras> activeCameras)
{
    // Load the palette asset and add two colors to it.
    auto palette = assets->write(PaletteAsset);
    auto black = palette->add({{0.1F, 0.1F, 0.1F, 1.0F}});
    auto white = palette->add({{0.9F, 0.9F, 0.9F, 1.0F}});

    // Set the renderer's palette to the one we just modified.
    (*renderer)->setPalette(*palette);

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

    cmds.create(RenderableGrid{floorAsset, floorOffset}, LocalToWorld{}, Scale{4.0F});

    // Spawn the camera entity.
    activeCameras->entities[0] =
        cmds.create(Camera{60.0F, 0.1F, 1000.0F}, LocalToWorld{})
            .add(Position{{0.0F, 120.0F, -200.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    // Spawn the sun.
    cmds.create(DirectionalLight{glm::vec3(1.0F), 1.0F}, LocalToWorld{},
                Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
}

static void spawn(Commands cmds, Write<Spawner> spawner, Read<DeltaTime> deltaTime, Read<Assets> assets)
{
    spawner->timer += deltaTime->value;
    if (spawner->timer >= 1.0F && spawner->y < 2)
    {
        auto car = assets->read(CarAsset);
        glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

        cmds.create(Car{}, RenderableGrid{CarAsset, offset}, LocalToWorld{})
            .add(Position{{80.0F * static_cast<float>(spawner->x), 0.0F, 80.0F * static_cast<float>(spawner->y)}})
            .add(Rotation{});

        spawner->timer = 0;
        spawner->x += 1;
        if (spawner->x == 2)
        {
            spawner->y++;
            spawner->x = -1;
        }
    }
}

static void move(Query<Write<Car>, Write<Position>, Write<Rotation>> query, Read<DeltaTime> deltaTime)
{
    for (auto [entity, car, position, rotation] : query)
    {
        car->vel = rotation->quat * glm::vec3(0.0F, 0.0F, 50.0F);
        car->angVel = 3.0F;

        position->vec += deltaTime->value * car->vel;
        rotation->quat = glm::angleAxis(deltaTime->value * car->angVel, glm::vec3(0.0F, 1.0F, 0.0F)) * rotation->quat;
    }
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(settingsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addComponent<Car>();
    cubos.addResource<Spawner>();

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(setup).tagged("cubos.assets").after("cubos.renderer.init");
    cubos.system(spawn);
    cubos.system(move);

    cubos.run();
}