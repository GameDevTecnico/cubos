#include "cubos/ecs/storage.hpp"
#include <iostream>

#include <cubos/ecs/world.hpp>

using namespace cubos;

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

int main()
{
    ecs::World world;
    world.registerComponent<Player>();
    world.registerComponent<Position>();
    world.registerComponent<Velocity>();

    auto player = world.create();
    world.addComponent<Position>(player, {0, 5, 1});
    world.addComponent<Velocity>(player, {0, 3, 1});
    world.addComponent<Player>(player);

    auto e2 = world.create<Position>({0, 8, 7});
    auto e3 = world.create(Position{0, 15, 7}, Velocity{0, 4, 1});
    auto e4 = world.create(Position{0, 12, 7}, Velocity{0, 5, 1});

    world.removeComponents<Position, Velocity>(e3);

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
