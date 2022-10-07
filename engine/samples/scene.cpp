#include <cubos/core/log.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/data/deserializer.hpp>

#include <cubos/core/memory/stream.hpp>

#include <cubos/core/ecs/vec_storage.hpp>
#include <cubos/core/ecs/registry.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/system.hpp>
#include <cubos/core/ecs/dispatcher.hpp>

#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/scene.hpp>

using namespace cubos;
using namespace engine;
using namespace core::ecs;
using namespace core::data;
using namespace core::memory;

struct Num
{
    using Storage = VecStorage<Num>;
    int value;
};

struct Parent
{
    using Storage = VecStorage<Parent>;
    Entity entity;
};

namespace cubos::core::data
{
    static void serialize(Serializer& ser, const Num& num, const char* name)
    {
        ser.write(num.value, name);
    }

    static void deserialize(Deserializer& des, Num& num)
    {
        des.read(num.value);
    }

    static void serialize(Serializer& ser, const Parent& parent, const SerializationMap<Entity, std::string>* map,
                          const char* name)
    {
        ser.write(parent.entity, map, name);
    }

    static void deserialize(Deserializer& des, Parent& parent, const SerializationMap<Entity, std::string>* map)
    {
        des.read(parent.entity, map);
    }
} // namespace cubos::core::data

CUBOS_REGISTER_COMPONENT(Num, "Num");
CUBOS_REGISTER_COMPONENT(Parent, "Parent");

void setup(Commands& cmds, std::shared_ptr<data::AssetManager>& assetManager)
{
    // Load and spawn the scene.
    auto scene = assetManager->load<data::Scene>("scenes/main");
    auto root = cmds.create().entity();
    cmds.spawn(scene->blueprint).add("main", Parent{root});
}

void printStuff(Query<const Parent*, const Num*> query)
{
    for (auto [entity, parent, num] : query)
    {
        Stream::stdOut.printf("Entity {}", entity.index);

        if (parent != nullptr)
        {
            Stream::stdOut.printf(" (Parent = {})", parent->entity.index);
        }

        if (num != nullptr)
        {
            Stream::stdOut.printf(" (Num = {})", num->value);
        }

        Stream::stdOut.put('\n');
    }
}

int main(int argc, char** argv)
{
    core::initializeLogger();
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    // Initialize the asset manager.
    auto assetManager = std::make_shared<data::AssetManager>();
    assetManager->registerType<data::Scene>();
    assetManager->importMeta(FileSystem::find("/assets/"));

    // Create an ECS world.
    auto world = World();
    world.registerResource<std::shared_ptr<data::AssetManager>>(assetManager);
    world.registerComponent<Num>();
    world.registerComponent<Parent>();

    // Create the dispatcher and register the systems.
    Dispatcher dispatcher;
    dispatcher.addSystem(setup, "Setup");
    dispatcher.addSystem(printStuff, "End");

    // Create the command buffer and run the dispatcher.
    auto cmds = Commands(world);
    dispatcher.callSystems(world, cmds);
}
