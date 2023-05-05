#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

#include "components.hpp"

using cubos::core::Settings;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::World;
using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("f0d86ba8-5f34-440f-a180-d9d12c8e8b91");

static void settings(Settings& settings)
{
    settings.setString("assets.path", SAMPLE_ASSETS_FOLDER);
}

static void spawnScene(Commands commands, const Assets& assets)
{
    auto sceneRead = assets.read(SceneAsset);
    commands.spawn(sceneRead->blueprint);
}

static void printStuff(World& world)
{
    using cubos::core::data::Context;
    using cubos::core::data::DebugSerializer;
    using cubos::core::data::SerializationMap;
    using cubos::core::memory::Stream;

    // Setup a context with a serialization map which simply converts the entity indices to a string.
    Context context{};
    context.push(SerializationMap<Entity, std::string>{[](const Entity& entity, std::string& string) {
                                                           string = std::to_string(entity.index);
                                                           return true;
                                                       },
                                                       [](Entity&, const std::string&) { return false; }});

    for (auto entity : world)
    {
        auto name = std::to_string(entity.index);
        auto ser = DebugSerializer(Stream::stdOut);
        auto pkg = world.pack(entity, context);
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
