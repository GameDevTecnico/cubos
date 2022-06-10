#include <iostream>

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/world_view.hpp>
#include <cubos/core/ecs/vec_storage.hpp>
#include <cubos/core/ecs/map_storage.hpp>
#include <cubos/core/ecs/null_storage.hpp>
#include <cubos/core/ecs/system.hpp>
#include <cubos/core/ecs/iterating_system.hpp>

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

class Spawner : public ecs::System
{
public:
    void init(ecs::World& world) override
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
};

class PrintPositions : public ecs::IteratingSystem<Position, Velocity>
{
public:
    void process(ecs::World& world, uint64_t entity, Position& pos, Velocity&) override
    {
        std::cout << pos.y << std::endl;
    }
};

class PrintPositions2 : public ecs::IteratingSystem<Player, Position, Velocity>
{
public:
    void process(ecs::World& world, uint64_t entity, Player&, Position& pos, Velocity&) override
    {
        std::cout << pos.y << std::endl;
    }
};

int main()
{
    ecs::World world;
    world.registerComponent<Player>();
    world.registerComponent<Position>();
    world.registerComponent<Velocity>();

    Spawner spawner;
    PrintPositions printPositions;
    PrintPositions2 printPositions2;
    spawner.init(world);
    printPositions.update(world);
    printPositions2.update(world);
}
