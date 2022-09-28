#include <cubos/core/ecs/entity_manager.hpp>

using namespace cubos::core::ecs;

void cubos::core::data::serialize(Serializer& serializer, const ecs::Entity& entity,
                                  const SerializationMap<ecs::Entity, std::string>* map, const char* name)
{
    if (entity.isNull())
    {
        serializer.write("null", name);
    }
    else
    {
        serializer.write(map->getId(entity), name);
    }
}

void cubos::core::data::deserialize(Deserializer& deserializer, ecs::Entity& entity,
                                    const SerializationMap<ecs::Entity, std::string>* map)
{
    std::string name;
    deserializer.read(name);
    if (name == "null")
    {
        entity = Entity();
    }
    else
    {
        entity = map->getRef(name);
    }
}

Entity::Entity()
{
    this->index = UINT32_MAX;
    this->generation = UINT32_MAX;
}

Entity::Entity(uint32_t index, uint32_t generation) : index(index), generation(generation)
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

EntityManager::Iterator::Iterator(const EntityManager& e, const Entity::Mask m) : manager(e), mask(m)
{
    if (!m.test(0))
    {
        abort(); // You can't iterate over invalid entities.
    }

    this->archetypeIt = this->manager.archetypes.begin();
    while (this->archetypeIt != this->manager.archetypes.end() &&
           ((this->archetypeIt->first & this->mask) != this->mask || this->archetypeIt->second.empty()))
    {
        ++this->archetypeIt;
    }

    if (this->archetypeIt != this->manager.archetypes.end())
    {
        this->entityIt = this->archetypeIt->second.begin();
    }
}

EntityManager::Iterator::Iterator(const EntityManager& e) : manager(e)
{
    this->archetypeIt = this->manager.archetypes.end();
}

Entity EntityManager::Iterator::operator*() const
{
    return Entity(*this->entityIt, this->manager.entities[*this->entityIt].generation);
}

bool EntityManager::Iterator::operator==(const Iterator& other) const
{
    if (other.archetypeIt == this->manager.archetypes.end())
    {
        return this->archetypeIt == this->manager.archetypes.end();
    }
    else
    {
        return this->archetypeIt == other.archetypeIt && this->entityIt == other.entityIt;
    }
}

bool EntityManager::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

EntityManager::Iterator& EntityManager::Iterator::operator++()
{
    if (this->archetypeIt != this->manager.archetypes.end())
    {
        if (this->entityIt != this->archetypeIt->second.end())
        {
            ++this->entityIt;
        }

        if (this->entityIt == this->archetypeIt->second.end())
        {
            // Move to the next archetype.
            do
            {
                ++this->archetypeIt;
            } while (this->archetypeIt != this->manager.archetypes.end() &&
                     ((this->archetypeIt->first & this->mask) != this->mask || this->archetypeIt->second.empty()));

            if (this->archetypeIt != this->manager.archetypes.end())
            {
                this->entityIt = this->archetypeIt->second.begin();
            }
        }
    }

    return *this;
}

EntityManager::EntityManager(size_t initialCapacity)
{
    this->entities.reserve(initialCapacity);
    for (size_t i = 0; i < initialCapacity; ++i)
    {
        this->entities.emplace_back(0, 1);
        this->availableEntities.push(i);
    }
}

Entity EntityManager::create(Entity::Mask mask)
{
    if (this->availableEntities.empty())
    {
        // Expand the entity pool.
        size_t oldSize = this->entities.size();
        this->entities.reserve(oldSize * 2);
        for (size_t i = oldSize; i < oldSize * 2; ++i)
        {
            this->entities.emplace_back(0, 0);
            this->availableEntities.push(i);
        }
    }

    uint32_t index = this->availableEntities.front();
    this->availableEntities.pop();
    this->entities[index].mask = mask;
    if (mask.any() && mask.test(0))
    {
        this->archetypes[mask].insert(index);
    }

    return Entity(index, this->entities[index].generation);
}

void EntityManager::destroy(Entity entity)
{
    this->setMask(entity, 0);
    this->entities[entity.index].generation += 1;
    this->availableEntities.push(entity.index);
}

void EntityManager::setMask(Entity entity, Entity::Mask mask)
{
    if (this->entities[entity.index].mask != mask)
    {
        if (this->entities[entity.index].mask.any() && this->entities[entity.index].mask.test(0))
            this->archetypes[this->entities[entity.index].mask].erase(entity.index);
        this->entities[entity.index].mask = mask;
        if (mask.any() && mask.test(0))
            this->archetypes[mask].insert(entity.index);
    }
}

const Entity::Mask& EntityManager::getMask(Entity entity) const
{
    return this->entities[entity.index].mask;
}

bool EntityManager::isValid(Entity entity) const
{
    return entity.index < this->entities.size() && this->entities[entity.index].generation == entity.generation;
}

EntityManager::Iterator EntityManager::withMask(Entity::Mask mask) const
{
    return Iterator(*this, mask);
}

EntityManager::Iterator EntityManager::end() const
{
    return Iterator(*this);
}
