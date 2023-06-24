#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

#include "components.hpp"

using cubos::core::Settings;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Read;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("f0d86ba8-5f34-440f-a180-d9d12c8e8b91");

static void settings(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void spawnScene(Commands commands, Read<Assets> assets)
{
    auto sceneRead = assets->read(SceneAsset);
    commands.spawn(sceneRead->blueprint);
}

static void printStuff(Read<World> world)
{
    using cubos::core::old::data::Context;
    using cubos::core::old::data::DebugSerializer;
    using cubos::core::old::data::SerializationMap;
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

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);

    cubos.addPlugin(scenePlugin);

    cubos.addComponent<Num>();
    cubos.addComponent<Parent>();

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(spawnScene).tagged("spawn").tagged("cubos.assets");
    cubos.startupSystem(printStuff).afterTag("spawn");
    cubos.run();
}
