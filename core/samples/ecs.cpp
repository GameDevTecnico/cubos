#include <iostream>

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/vec_storage.hpp>
#include <cubos/core/ecs/map_storage.hpp>
#include <cubos/core/ecs/null_storage.hpp>
#include <cubos/core/ecs/system.hpp>
#include <cubos/core/ecs/commands.hpp>
#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/registry.hpp>
#include <cubos/core/ecs/dispatcher.hpp>

using namespace cubos::core;

struct Player
{
    using Storage = ecs::NullStorage<Player>;
};

struct Position
{
    using Storage = ecs::VecStorage<Position>;
    float x, y, z;
};

struct Velocity
{
    using Storage = ecs::MapStorage<Velocity>;
    float x, y, z;
};

struct Parent
{
    using Storage = ecs::VecStorage<Parent>;
    ecs::Entity entity;
};

namespace cubos::core::data
{
    void serialize(Serializer& ser, const Player& player, const char* name)
    {
        // Do nothing.
    }

    void deserialize(Deserializer& des, Player& player)
    {
        // Do nothing.
    }

    void serialize(Serializer& ser, const Position& position, const char* name)
    {
        ser.beginObject(name);
        ser.write(position.x, "x");
        ser.write(position.y, "y");
        ser.write(position.z, "z");
        ser.endObject();
    }

    void deserialize(Deserializer& des, Position& position)
    {
        des.beginObject();
        des.read(position.x);
        des.read(position.y);
        des.read(position.z);
        des.endObject();
    }

    void serialize(Serializer& ser, const Velocity& velocity, const char* name)
    {
        ser.beginObject(name);
        ser.write(velocity.x, "x");
        ser.write(velocity.y, "y");
        ser.write(velocity.z, "z");
        ser.endObject();
    }

    void deserialize(Deserializer& des, Velocity& velocity)
    {
        des.beginObject();
        des.read(velocity.x);
        des.read(velocity.y);
        des.read(velocity.z);
        des.endObject();
    }

    void serialize(Serializer& ser, const Parent& parent, const SerializationMap<ecs::Entity, std::string>& map,
                   const char* name)
    {
        ser.write(parent.entity, map, name);
    }

    void deserialize(Deserializer& des, Parent& parent, const SerializationMap<ecs::Entity, std::string>& map)
    {
        des.read(parent.entity, map);
    }
} // namespace cubos::core::data

CUBOS_REGISTER_COMPONENT(Player, "Player");
CUBOS_REGISTER_COMPONENT(Position, "Position");
CUBOS_REGISTER_COMPONENT(Velocity, "Velocity");
CUBOS_REGISTER_COMPONENT(Parent, "Parent");

void spawner(ecs::Commands& cmds)
{
    cmds.create().add<Position>({0, 5, 1}).add(Velocity{0, 3, 1}, Player{});

    cmds.create<Position, Player>({0, 8, 7}, {});
    cmds.create(Position{0, 15, 7}, Velocity{0, 4, 1});
    cmds.create(Position{0, 12, 7}, Velocity{0, 5, 1});

    ecs::Blueprint epicBlueprint;
    ecs::Entity main = epicBlueprint.create("main", Position{0, 0, 0}, Velocity{0, 0, 0});
    epicBlueprint.create("sub", Parent{main});

    cmds.spawn(epicBlueprint).add("main", Position(1, 1, 1));
    cmds.spawn(epicBlueprint).add("main", Position(2, 2, 2));
    cmds.spawn(epicBlueprint).add("main", Position(3, 3, 3));
}

struct DeltaTime
{
    float dt;
};

struct MyResource
{
    int val;
};

void printPositions(ecs::Query<Position&, const Velocity&> query)
{
    std::cout << "positions w/velocity:" << std::endl;
    for (auto [entity, position, velocity] : query)
    {
        std::cout << position.x << ' ' << position.y << ' ' << position.z << std::endl;
    }
}

void printPlayerPosition(ecs::Query<const Player&, Position&> query)
{
    std::cout << "player positions:" << std::endl;
    for (auto [entity, player, position] : query)
    {
        std::cout << position.x << ' ' << position.y << ' ' << position.z << std::endl;
    }
}

void mySystem(DeltaTime& dt, const MyResource& res)
{
    std::cout << "mySystem: " << dt.dt << " " << res.val << std::endl;
}

int main()
{
    initializeLogger();
    ecs::World world;
    world.registerResource<DeltaTime>(DeltaTime{1.0f});
    world.registerResource<MyResource>(MyResource{0});
    world.registerComponent<Player>();
    world.registerComponent<Position>();
    world.registerComponent<Velocity>();
    world.registerComponent<Parent>();

    ecs::Dispatcher dispatcher;
    auto cmds = ecs::Commands(world);
    dispatcher.addSystem(mySystem, "Main");
    dispatcher.addSystem(printPositions, "Transform");
    dispatcher.setDefaultStage("Main", ecs::Dispatcher::Direction::After);
    dispatcher.addSystem(printPlayerPosition, "New");
    dispatcher.setDefaultStage("Main", ecs::Dispatcher::Direction::Before);
    dispatcher.addSystem(
        [](const DeltaTime& dt, MyResource& res) { std::cout << "lambda: " << dt.dt << " " << res.val << std::endl; },
        "PreProcess");
    dispatcher.putStageBefore("PreProcess", "Transform");
    // call systems on dispatcher
    dispatcher.callSystems(world, cmds);
}
