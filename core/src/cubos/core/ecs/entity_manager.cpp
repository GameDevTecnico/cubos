#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serialization_map.hpp>
#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/ecs/entity_manager.hpp>

using namespace cubos::core::ecs;
using namespace cubos::core::data;

template <>
void cubos::core::data::serialize<Entity>(Serializer& ser, const Entity& obj, const char* name)
{
    if (ser.context().has<SerializationMap<Entity, std::string>>())
    {
        if (obj.isNull())
        {
            ser.write("null", name);
            return;
        }

        auto& map = ser.context().get<SerializationMap<Entity, std::string>>();
        ser.write(map.getId(obj), name);
    }
    else
    {
        ser.beginObject(name);
        ser.write(obj.index, "index");
        ser.write(obj.generation, "generation");
        ser.endObject();
    }
}

template <>
void cubos::core::data::deserialize<Entity>(Deserializer& des, Entity& obj)
{
    if (des.context().has<SerializationMap<Entity, std::string>>())
    {
        std::string name;
        des.read(name);
        if (name == "null")
        {
            obj = Entity();
            return;
        }

        auto& map = des.context().get<SerializationMap<Entity, std::string>>();
        if (map.hasId(name))
        {
            obj = map.getRef(name);
        }
        else
        {
            CUBOS_WARN("No such entity '{}'", name);
            des.fail();
        }
    }
    else
    {
        des.beginObject();
        des.read(obj.index);
        des.read(obj.generation);
        des.endObject();
    }
}

Entity::Entity()
{
    this->index = UINT32_MAX;
    this->generation = UINT32_MAX;
}

Entity::Entity(uint32_t index, uint32_t generation)
    : index(index)
    , generation(generation)
{
    // Do nothing.
}

bool Entity::operator==(const Entity& entity) const
{
    return this->index == entity.index && this->generation == entity.generation;
}

bool Entity::operator!=(const Entity& entity) const
{
    return !(*this == entity);
}

bool Entity::isNull() const
{
    return this->index == UINT32_MAX && this->generation == UINT32_MAX;
}

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
