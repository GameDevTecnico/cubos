#include <cubos/core/ecs/world.hpp>

using namespace cubos::core::ecs;

World::~World()
{
    for (auto* storage : storages)
    {
        delete storage;
    }
}

void World::remove(uint64_t entity)
{
    uint32_t entityIndex = (uint32_t)entity;
    uint32_t entityVersion = entity >> 32;
    if (entityVersion != entityData[entityIndex * elementsPerEntity])
        return;

    entityData[entityIndex * elementsPerEntity] = entityVersion + 1;
    for (size_t i = 1; i < elementsPerEntity; i++)
    {
        entityData[entityIndex * elementsPerEntity + i] = 0;
    }
}
