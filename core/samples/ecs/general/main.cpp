#include <iostream>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/component/map_storage.hpp>
#include <cubos/core/ecs/component/null_storage.hpp>
#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/component/vec_storage.hpp>
#include <cubos/core/ecs/system/commands.hpp>
#include <cubos/core/ecs/system/dispatcher.hpp>
#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/ecs/world.hpp>

using namespace cubos::core;

struct Player
{
};

struct Position
{
    float x, y, z;
};

struct Velocity
{
    float x, y, z;
};

struct Parent
{
    ecs::Entity entity;
};

namespace cubos::core::data::old
{
    void serialize(Serializer& /*unused*/, const Player& /*unused*/, const char* /*unused*/)
    {
        // Do nothing.
    }

    void deserialize(Deserializer& /*unused*/, Player& /*unused*/)
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

    void serialize(Serializer& ser, const Parent& parent, const char* name)
    {
        ser.write(parent.entity, name);
    }

    void deserialize(Deserializer& des, Parent& parent)
    {
        des.read(parent.entity);
    }
} // namespace cubos::core::data::old

CUBOS_REGISTER_COMPONENT(Player, ecs::NullStorage<Player>, "Player")
CUBOS_REGISTER_COMPONENT(Position, ecs::VecStorage<Position>, "Position")
CUBOS_REGISTER_COMPONENT(Velocity, ecs::MapStorage<Velocity>, "Velocity")
CUBOS_REGISTER_COMPONENT(Parent, ecs::VecStorage<Parent>, "Parent")

void spawner(ecs::Commands cmds)
{
    cmds.create().add<Position>({0, 5, 1}).add(Velocity{0, 3, 1}, Player{});

    cmds.create<Position, Player>({0, 8, 7}, {});
    cmds.create(Position{0, 15, 7}, Velocity{0, 4, 1});
    cmds.create(Position{0, 12, 7}, Velocity{0, 5, 1});

    ecs::Blueprint epicBlueprint;
    ecs::Entity main = epicBlueprint.create("main", Position{0, 0, 0}, Velocity{0, 0, 0});
    epicBlueprint.create("sub", Parent{main});

    cmds.spawn(epicBlueprint).add("main", Position{1, 1, 1});
    cmds.spawn(epicBlueprint).add("main", Position{2, 2, 2});
    cmds.spawn(epicBlueprint).add("main", Position{3, 3, 3});
}

struct DeltaTime
{
    float dt;
};

struct MyResource
{
    int val;
};

void printPositions(ecs::Query<ecs::Write<Position>, ecs::Read<Velocity>> query)
{
    std::cout << "positions w/velocity:" << std::endl;
    for (auto [entity, position, velocity] : query)
    {
        std::cout << position->x << ' ' << position->y << ' ' << position->z << std::endl;
    }
}

void printPlayerPosition(ecs::Query<ecs::Read<Player>, ecs::Write<Position>> query)
{
    std::cout << "player positions:" << std::endl;
    for (auto [entity, player, position] : query)
    {
        std::cout << position->x << ' ' << position->y << ' ' << position->z << std::endl;
    }
}

void mySystem(ecs::Write<DeltaTime> dt, ecs::Read<MyResource> res)
{
    std::cout << "mySystem: " << dt->dt << " " << res->val << std::endl;
}

int main()
{
    initializeLogger();
    ecs::World world;
    world.registerResource<DeltaTime>(DeltaTime{1.0F});
    world.registerResource<MyResource>(MyResource{0});
    world.registerComponent<Player>();
    world.registerComponent<Position>();
    world.registerComponent<Velocity>();
    world.registerComponent<Parent>();

    ecs::Dispatcher dispatcher;
    auto cmds = ecs::CommandBuffer(world);

    dispatcher.addSystem(spawner);
    dispatcher.systemSetBeforeTag("PreProcess");

    dispatcher.addTag("Main");
    dispatcher.addSystem(mySystem);
    dispatcher.systemAddTag("Main");

    dispatcher.addTag("Transform");
    dispatcher.tagSetAfterTag("Main");
    dispatcher.addSystem(printPositions);
    dispatcher.systemAddTag("Transform");

    dispatcher.addTag("New");
    dispatcher.tagSetAfterTag("Main");
    dispatcher.addSystem(printPlayerPosition);
    dispatcher.systemAddTag("New");

    dispatcher.addTag("PreProcess");
    dispatcher.tagSetBeforeTag("Main");
    dispatcher.tagSetBeforeTag("Transform");
    dispatcher.addSystem([](ecs::Read<DeltaTime> dt, ecs::Write<MyResource> res) {
        std::cout << "lambda: " << dt->dt << " " << res->val << std::endl;
    });
    dispatcher.systemAddTag("PreProcess");

    dispatcher.compileChain();
    // call systems on dispatcher
    dispatcher.callSystems(world, cmds);
}
