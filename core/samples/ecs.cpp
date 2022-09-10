#include <iostream>

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/vec_storage.hpp>
#include <cubos/core/ecs/map_storage.hpp>
#include <cubos/core/ecs/null_storage.hpp>
#include <cubos/core/ecs/system.hpp>
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

void spawner(ecs::World& world)
{
    auto player = world.create();
    world.addComponent<Position>(player, {0, 5, 1});
    world.addComponent<Velocity>(player, {0, 3, 1});
    world.addComponent<Player>(player);

    auto e2 = world.create<Position>({0, 8, 7});
    auto e3 = world.create(Position{0, 15, 7}, Velocity{0, 4, 1});
    auto e4 = world.create(Position{0, 12, 7}, Velocity{0, 5, 1});

    world.removeComponents<Position, Velocity>(e3);
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
    for (auto [entity, position, velocity] : query)
    {
        std::cout << position.y << std::endl;
    }
}

void printPlayerPosition(ecs::Query<const Player&, Position&, const Velocity&> query)
{
    for (auto [entity, player, position, velocity] : query)
    {
        std::cout << position.y << std::endl;
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

    spawner(world);
    // create dispatcher
    ecs::Dispatcher dispatcher;
    // register systems wrappers on dispatcher
    dispatcher.registerSystem(mySystem, "Main");
    dispatcher.registerSystem(printPositions, "Transform");
    dispatcher.setDefaultStage("Main", ecs::Dispatcher::Direction::AFTER);
    dispatcher.registerSystem(printPlayerPosition, "New");
    dispatcher.setDefaultStage("Main", ecs::Dispatcher::Direction::BEFORE);
    dispatcher.registerSystem(
        [](const DeltaTime& dt, MyResource& res) { std::cout << "lambda: " << dt.dt << " " << res.val << std::endl; },
        "PreProcess");
    dispatcher.putStageBefore("PreProcess", "Transform");
    // call systems on dispatcher
    dispatcher.callSystems(world);
}
