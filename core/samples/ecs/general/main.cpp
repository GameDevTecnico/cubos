#include <iostream>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/ecs/system/dispatcher.hpp>
#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
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
    return ecs::ComponentTypeBuilder<Player>("Player").build();
}

CUBOS_REFLECT_IMPL(Position)
{
    return ecs::ComponentTypeBuilder<Position>("Position")
        .withField("x", &Position::x)
        .withField("y", &Position::y)
        .withField("z", &Position::z)
        .build();
}

CUBOS_REFLECT_IMPL(Velocity)
{
    return ecs::ComponentTypeBuilder<Velocity>("Velocity")
        .withField("x", &Velocity::x)
        .withField("y", &Velocity::y)
        .withField("z", &Velocity::z)
        .build();
}

CUBOS_REFLECT_IMPL(Parent)
{
    return ecs::ComponentTypeBuilder<Parent>("Parent").withField("entity", &Parent::entity).build();
}

void spawner(ecs::Commands cmds)
{
    cmds.create().add(Position{0, 5, 1}).add(Velocity{0, 3, 1}).add(Player{});

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

void printPositions(ecs::Query<Position&, const Velocity&> query)
{
    std::cout << "positions w/velocity:" << std::endl;
    for (auto [position, velocity] : query)
    {
        std::cout << position.x << ' ' << position.y << ' ' << position.z << std::endl;
    }
}

void printPlayerPosition(ecs::Query<const Player&, Position&> query)
{
    std::cout << "player positions:" << std::endl;
    for (auto [player, position] : query)
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
    dispatcher.addSystem(
        [](const DeltaTime& dt, MyResource& res) { std::cout << "lambda: " << dt.dt << " " << res.val << std::endl; });
    dispatcher.systemAddTag("PreProcess");

    dispatcher.compileChain();
    // call systems on dispatcher
    dispatcher.callSystems(world, cmds);
}
