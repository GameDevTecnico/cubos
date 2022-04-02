#include <iostream>

#include <cubos/ecs/world.hpp>

using namespace cubos;

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

int main()
{
    ecs::World world;
    world.registerComponent<Player>(new ecs::NullStorage<Player>());
    world.registerComponent<Position>(new ecs::VecStorage<Position>());
    world.registerComponent<Velocity>(new ecs::MapStorage<Velocity>());

    auto player = world.create();
    world.addComponent<Position>(player, Position{0, 5, 1});
    world.addComponent<Velocity>(player, Velocity{0, 3, 1});
    world.addComponent<Player>(player, Player{});

    auto e2 = world.create();
    world.addComponent<Position>(e2, Position{0, 8, 7});

    auto e3 = world.create();
    world.addComponent<Position>(e3, Position{0, 15, 7});
    world.addComponent<Velocity>(e3, Velocity{0, 4, 1});

    auto e4 = world.create();
    world.addComponent<Position>(e4, Position{0, 12, 7});
    world.addComponent<Velocity>(e4, Velocity{0, 5, 1});

    for (auto entity : ecs::WorldView<Position, Velocity>(world))
    {
        auto* pos = world.getComponent<Position>(entity);
        std::cout << pos->y << std::endl;
    }

    for (auto entity : ecs::WorldView<Player, Position, Velocity>(world))
    {
        auto* pos = world.getComponent<Position>(entity);
        std::cout << pos->y << std::endl;
    }
}
