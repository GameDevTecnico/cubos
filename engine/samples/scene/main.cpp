#include <cubos/core/data/old/debug_serializer.hpp>
#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

#include "components.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Read;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;

using namespace cubos::engine;

/// [Setting the asset]
static const Asset<Scene> SceneAsset = AnyAsset("f0d86ba8-5f34-440f-a180-d9d12c8e8b91");
/// [Setting the asset]

static void settings(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

/// [Spawning the scene]
static void spawnScene(Commands commands, Read<Assets> assets)
{
    auto sceneRead = assets->read(SceneAsset);
    commands.spawn(sceneRead->blueprint);
}
/// [Spawning the scene]

/// [Displaying the scene]
static void printStuff(Read<World> world)
{
    using cubos::core::data::old::Context;
    using cubos::core::data::old::DebugSerializer;
    using cubos::core::data::old::SerializationMap;
    using cubos::core::memory::Stream;

    for (auto entity : *world)
    {
        auto name = std::to_string(entity.index);
        auto ser = DebugSerializer(Stream::stdOut);
        auto pkg = world->pack(entity);
        ser.write(pkg, name.c_str());
        Stream::stdOut.put('\n');
    }
}
/// [Displaying the scene]

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);

    /// [Adding the plugin]
    cubos.addPlugin(scenePlugin);
    /// [Adding the plugin]

    cubos.addComponent<Num>();
    cubos.addComponent<Parent>();

    cubos.startupSystem(settings).tagged("cubos.settings");

    /// [Adding the system]
    cubos.startupSystem(spawnScene).tagged("spawn").tagged("cubos.assets");
    /// [Adding the system]

    cubos.startupSystem(printStuff).after("spawn");
    cubos.run();
}
