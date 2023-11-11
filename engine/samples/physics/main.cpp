#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/core/ecs/system/query.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::ecs::Query;

using namespace cubos::engine;

static const Asset<VoxelGrid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c80");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc220");

struct MaxTime
{
    float max = 1.0F;
    float current = 0.0F;
};

static void settingsSystem(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void setPaletteSystem(Read<Assets> assets, Write<Renderer> renderer)
{
    // Read the palette's data and pass it to the renderer.
    auto palette = assets->read(PaletteAsset);
    (*renderer)->setPalette(*palette);
}

static void spawnCameraSystem(Commands cmds, Write<ActiveCameras> activeCameras)
{
    // Spawn the camera entity.
    activeCameras->entities[0] =
        cmds.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 1000.0F})
            .add(Position{{50.0F, 50.0F, 50.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{-1.0F, -1.0F, -1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
}

static void spawnLightSystem(Commands cmds)
{
    // Spawn the sun.
    cmds.create()
        .add(DirectionalLight{glm::vec3(1.0F), 1.0F})
        .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
}

static void spawnCarSystem(Commands cmds, Read<Assets> assets)
{
    // Calculate the necessary offset to center the model on (0, 0, 0).
    auto car = assets->read(CarAsset);
    glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

    // Create the car entity
    cmds.create().add(RenderableGrid{CarAsset, offset})
                 .add(Position{{0.0F, 0.0F, 0.0F}})
                 .add(PreviousPosition{{0.0F, 0.0F, 0.0F}})
                 .add(PhysicsVelocity{.velocity = {0.0F, 0.0F, 0.0F},
                                      .force = {0.0F, 0.0F, 0.0F},
                                      .impulse = {0.0F, 0.0F, 0.0F}})
                 .add(PhysicsMass{.mass = 500.0F, .inverseMass = 1.0F/500.0F})
                 .add(AccumulatedCorrection{{0.0F, 0.0F, 0.0F}})
                 .add(LocalToWorld{});
}

static void pushCarSystem(Query<Write<PhysicsVelocity>> query, Write<MaxTime> time, Read<DeltaTime> deltaTime) {
    for (auto [entity, velocity] : query) {
        if (time->current < time->max) {
            if (time->current == 0.0F)
            {
                velocity->impulse += glm::vec3(0.0F, 5000.0F, 0.0F);
            }
            velocity->force = velocity->force + glm::vec3(0.0F, 0.0F, -5000.0F);
            time->current = time->current + deltaTime->value;
        }
    }
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.addResource<MaxTime>();

    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addPlugin(physicsPlugin);

    cubos.startupSystem(settingsSystem).tagged("cubos.settings");
    cubos.startupSystem(spawnCameraSystem);
    cubos.startupSystem(spawnLightSystem);
    cubos.startupSystem(setPaletteSystem).after("cubos.renderer.init");
    cubos.startupSystem(spawnCarSystem).after("cubos.settings").after("cubos.assets.bridge");
    
    cubos.system(pushCarSystem).tagged("cubos.physics.apply_forces");

    cubos.run();
}
