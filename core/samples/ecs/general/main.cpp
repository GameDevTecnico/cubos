#include <iostream>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/system/commands.hpp>
#include <cubos/core/ecs/system/dispatcher.hpp>
#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core;

using reflection::ConstructibleTrait;
using reflection::Type;

struct Player
{
    CUBOS_REFLECT;
};

struct Position
{
    CUBOS_REFLECT;

    float x, y, z;
};

struct Velocity
{
    CUBOS_REFLECT;

    float x, y, z;
};

struct Parent
{
    CUBOS_REFLECT;

    ecs::Entity entity;
};

CUBOS_REFLECT_IMPL(Player)
{
    return Type::create("Player").with(ConstructibleTrait::typed<Player>().withMoveConstructor().build());
}

CUBOS_REFLECT_IMPL(Position)
{
    return Type::create("Position").with(ConstructibleTrait::typed<Position>().withMoveConstructor().build());
}

CUBOS_REFLECT_IMPL(Velocity)
{
    return Type::create("Velocity").with(ConstructibleTrait::typed<Velocity>().withMoveConstructor().build());
}

CUBOS_REFLECT_IMPL(Parent)
{
    return Type::create("Parent").with(ConstructibleTrait::typed<Parent>().withMoveConstructor().build());
}

void spawner(ecs::Commands cmds)
{
    cmds.create().add<Position>({0, 5, 1}).add(Velocity{0, 3, 1}, Player{});

    cmds.create<Position, Player>({0, 8, 7}, {});
    cmds.create(Position{0, 15, 7}, Velocity{0, 4, 1});
    cmds.create(Position{0, 12, 7}, Velocity{0, 5, 1});

    ecs::Blueprint epicBlueprint{};
    auto main = epicBlueprint.create("main");
    epicBlueprint.add(main, Position{0, 0, 0}, Velocity{0, 0, 0});
    auto sub = epicBlueprint.create("sub");
    epicBlueprint.add(sub, Parent{main});

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
    Logger::level(Logger::Level::Critical);
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
