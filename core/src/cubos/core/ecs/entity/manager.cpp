#include <cubos/core/ecs/entity/manager.hpp>

using namespace cubos::core::ecs;

EntityManager::Iterator::Iterator(const EntityManager& e, const Entity::Mask m)
    : mManager(e)
    , mMask(m)
{
    if (!m.test(0))
    {
        abort(); // You can't iterate over invalid entities.
    }

    mArchetypeIt = mManager.mArchetypes.begin();
    while (mArchetypeIt != mManager.mArchetypes.end() &&
           ((mArchetypeIt->first & mMask) != mMask || mArchetypeIt->second.empty()))
    {
        ++mArchetypeIt;
    }

    if (mArchetypeIt != mManager.mArchetypes.end())
    {
        mEntityIt = mArchetypeIt->second.begin();
    }
}

EntityManager::Iterator::Iterator(const EntityManager& e)
    : mManager(e)
{
    mArchetypeIt = mManager.mArchetypes.end();
}

Entity EntityManager::Iterator::operator*() const
{
    return {*mEntityIt, mManager.mEntities[*mEntityIt].generation};
}

bool EntityManager::Iterator::operator==(const Iterator& other) const
{
    if (other.mArchetypeIt == mManager.mArchetypes.end())
    {
        return mArchetypeIt == mManager.mArchetypes.end();
    }

    return mArchetypeIt == other.mArchetypeIt && mEntityIt == other.mEntityIt;
}

bool EntityManager::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

EntityManager::Iterator& EntityManager::Iterator::operator++()
{
    if (mArchetypeIt != mManager.mArchetypes.end())
    {
        if (mEntityIt != mArchetypeIt->second.end())
        {
            ++mEntityIt;
        }

        if (mEntityIt == mArchetypeIt->second.end())
        {
            // Move to the next archetype.
            do
            {
                ++mArchetypeIt;
            } while (mArchetypeIt != mManager.mArchetypes.end() &&
                     ((mArchetypeIt->first & mMask) != mMask || mArchetypeIt->second.empty()));

            if (mArchetypeIt != mManager.mArchetypes.end())
            {
                mEntityIt = mArchetypeIt->second.begin();
            }
        }
    }

    return *this;
}

EntityManager::EntityManager(std::size_t initialCapacity)
{
    mEntities.reserve(initialCapacity);
    for (std::size_t i = 0; i < initialCapacity; ++i)
    {
        mEntities.push_back(EntityData{0, 1});
        mAvailableEntities.push(static_cast<uint32_t>(i));
    }
}

Entity EntityManager::create(Entity::Mask mask)
{
    if (mAvailableEntities.empty())
    {
        // Expand the entity pool.
        std::size_t oldSize = mEntities.size();
        mEntities.reserve(oldSize * 2);
        for (std::size_t i = oldSize; i < oldSize * 2; ++i)
        {
            mEntities.push_back(EntityData{0, 0});
            mAvailableEntities.push(static_cast<uint32_t>(i));
        }
    }

    uint32_t index = mAvailableEntities.front();
    mAvailableEntities.pop();
    mEntities[index].mask = mask;
    if (mask.any() && mask.test(0))
    {
        mArchetypes[mask].insert(index);
    }

    return {index, mEntities[index].generation};
}

void EntityManager::destroy(Entity entity)
{
    this->setMask(entity, 0);
    mEntities[entity.index].generation += 1;
    mAvailableEntities.push(entity.index);
}

void EntityManager::setMask(Entity entity, Entity::Mask mask)
{
    if (mEntities[entity.index].mask != mask)
    {
        if (mEntities[entity.index].mask.any() && mEntities[entity.index].mask.test(0))
        {
            mArchetypes[mEntities[entity.index].mask].erase(entity.index);
        }
        mEntities[entity.index].mask = mask;
        if (mask.any() && mask.test(0))
        {
            mArchetypes[mask].insert(entity.index);
        }
    }
}

const Entity::Mask& EntityManager::getMask(Entity entity) const
{
    return mEntities[entity.index].mask;
}

bool EntityManager::isValid(Entity entity) const
{
    return entity.index < mEntities.size() && mEntities[entity.index].generation == entity.generation;
}

bool EntityManager::isAlive(Entity entity) const
{
    return this->isValid(entity) && mEntities[entity.index].mask.test(0);
}

EntityManager::Iterator EntityManager::begin() const
{
    return {*this, Entity::Mask(1)};
}

EntityManager::Iterator EntityManager::withMask(Entity::Mask mask) const
{
    return {*this, mask};
}

EntityManager::Iterator EntityManager::end() const
{
    return {*this};
}
