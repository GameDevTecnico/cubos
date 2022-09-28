#include <cubos/core/ecs/world.hpp>

using namespace cubos::core::ecs;

World::World(size_t initialCapacity) : entityManager(initialCapacity)
{
    // Do nothing.
    // Edu: BAH!
}

void World::destroy(Entity entity)
{
    this->entityManager.destroy(entity);
    this->componentManager.removeAll(entity.index);
}
