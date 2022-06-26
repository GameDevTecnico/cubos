#include <cubos/core/ecs/world.hpp>

using namespace cubos::core::ecs;

size_t World::nextGlobalComponentId = 0;

World::World(size_t initialCapacity) : entityManager(initialCapacity)
{
    // Do nothing.
    // Edu: BAH!
}

World::~World()
{
    for (auto* storage : storages)
    {
        delete storage;
    }
}

void World::remove(Entity entity)
{
    this->entityManager.remove(entity);
    // TODO: remove from all storages.
}

EntityManager::Iterator World::end() const
{
    return this->entityManager.end();
}
