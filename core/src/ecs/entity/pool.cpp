#include <cubos/core/ecs/entity/pool.hpp>

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::Entity;
using cubos::core::ecs::EntityPool;

Entity EntityPool::create(ArchetypeId archetype)
{
    uint32_t index;
    if (mFree.empty())
    {
        index = static_cast<uint32_t>(mEntries.size());
        mEntries.emplace_back();
    }
    else
    {
        index = mFree.front();
        mFree.pop();
    }

    mEntries[index].archetype = archetype;
    return {index, mEntries[index].generation};
}

void EntityPool::destroy(uint32_t index)
{
    mEntries[index].generation += 1;
    mFree.push(index);
}

void EntityPool::archetype(uint32_t index, ArchetypeId archetype)
{
    mEntries[index].archetype = archetype;
}

ArchetypeId EntityPool::archetype(uint32_t index) const
{
    return mEntries[index].archetype;
}

uint32_t EntityPool::generation(uint32_t index) const
{
    return mEntries[index].generation;
}

bool EntityPool::contains(Entity entity) const
{
    return entity.index < mEntries.size() && mEntries[entity.index].generation == entity.generation;
}
