#include <cubos/ecs/world.hpp>

using namespace cubos::ecs;

World::~World()
{
    for (auto* storage : storages)
    {
        delete storage;
    }
}
