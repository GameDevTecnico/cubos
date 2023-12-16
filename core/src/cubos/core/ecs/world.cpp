#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

void World::registerComponent(const reflection::Type& type)
{
    CUBOS_TRACE("Registered component '{}'", type.name());
    mTypes.addComponent(type);
}

Entity World::create()
{
    auto entity = this->reserve();
    this->createAt(entity);
    return entity;
}

void World::createAt(Entity entity)
{
    // Check if the entity handle is valid.
    CUBOS_ASSERT(mEntityPool.contains(entity), "Entity is not reserved");

    // Set the archetype of the entity and check if it wasn't already set before.
    auto oldArchetype = mEntityPool.archetype(entity.index, ArchetypeId::Empty);
    CUBOS_ASSERT(oldArchetype == ArchetypeId::Invalid, "Entity has already been created");

    // Push the entity to the archetype's table.
    mTables.dense(ArchetypeId::Empty).pushBack(entity.index);
    CUBOS_DEBUG("Created entity {}", entity);
}

Entity World::reserve()
{
    return mEntityPool.create(ArchetypeId::Invalid);
}

void World::destroy(Entity entity)
{
    if (!mEntityPool.contains(entity))
    {
        CUBOS_DEBUG("Entity {} does not exist", entity);
        return;
    }

    auto archetype = mEntityPool.destroy(entity.index);
    mTables.dense(archetype).swapErase(entity.index);
    CUBOS_DEBUG("Destroyed entity {}", entity);
}

uint32_t World::generation(uint32_t index) const
{
    return mEntityPool.generation(index);
}

ArchetypeId World::archetype(Entity entity) const
{
    return mEntityPool.archetype(entity.index);
}

bool World::isAlive(Entity entity) const
{
    return mEntityPool.contains(entity) && mEntityPool.archetype(entity.index) != ArchetypeId::Invalid;
}

Types& World::types()
{
    return mTypes;
}

const Types& World::types() const
{
    return mTypes;
}

Tables& World::tables()
{
    return mTables;
}

const Tables& World::tables() const
{
    return mTables;
}

ArchetypeGraph& World::archetypeGraph()
{
    return mArchetypeGraph;
}

const ArchetypeGraph& World::archetypeGraph() const
{
    return mArchetypeGraph;
}

World::Components World::components(Entity entity)
{
    return Components{*this, entity};
}

World::ConstComponents World::components(Entity entity) const
{
    return ConstComponents{*this, entity};
}

World::Components::Components(World& world, Entity entity)
    : mWorld{world}
    , mEntity{entity}
{
    CUBOS_ASSERT(world.isAlive(entity));
}

bool World::Components::has(const reflection::Type& type) const
{
    auto archetype = mWorld.mEntityPool.archetype(mEntity.index);
    auto columnId = DenseColumnId::make(mWorld.mTypes.id(type));
    return mWorld.mArchetypeGraph.contains(archetype, columnId);
}

void* World::Components::get(const reflection::Type& type)
{
    auto archetype = mWorld.mEntityPool.archetype(mEntity.index);
    auto& table = mWorld.mTables.dense(archetype);
    auto columnId = DenseColumnId::make(mWorld.mTypes.id(type));
    return table.column(columnId).at(table.row(mEntity.index));
}

auto World::Components::begin() -> Iterator
{
    return Iterator{*this, false};
}

auto World::Components::end() -> Iterator
{
    return Iterator{*this, true};
}

auto World::Components::add(const reflection::Type& type, void* value) -> Components&
{
    auto typeId = mWorld.mTypes.id(type);
    CUBOS_ASSERT(mWorld.mTypes.isComponent(typeId), "Type '{}' is not registered as a component", type.name());
    auto columnId = DenseColumnId::make(typeId);

    auto oldArchetype = mWorld.mEntityPool.archetype(mEntity.index);
    auto& oldTable = mWorld.mTables.dense(oldArchetype);

    // If the old archetype already contains this component type, then we only need to overwrite
    // its existing entry in the table.
    if (mWorld.mArchetypeGraph.contains(oldArchetype, columnId))
    {
        oldTable.column(columnId).setMove(oldTable.row(mEntity.index), value);
        return *this;
    }

    // Otherwise, we'll need to move the entity to a new archetype.
    auto newArchetype = mWorld.mArchetypeGraph.with(oldArchetype, columnId);
    mWorld.mEntityPool.archetype(mEntity.index, newArchetype);

    // Get the table of the new archetype, which we may have to create.
    auto& newTable = mWorld.mTables.dense(newArchetype, mWorld.mArchetypeGraph, mWorld.mTypes);

    // Move the entity from the old table to the new one and add the new component.
    oldTable.swapMove(mEntity.index, newTable);
    newTable.column(columnId).pushMove(value);

    CUBOS_DEBUG("Added component {} to entity {}", type.name(), mEntity, mEntity);
    return *this;
}

auto World::Components::remove(const reflection::Type& type) -> Components&
{
    auto typeId = mWorld.mTypes.id(type);
    CUBOS_ASSERT(mWorld.mTypes.isComponent(typeId), "Type '{}' is not registered as a component", type.name());
    auto columnId = DenseColumnId::make(typeId);

    // If the old archetype doesn't contain this component type, then we don't do anything.
    auto oldArchetype = mWorld.mEntityPool.archetype(mEntity.index);
    if (!mWorld.mArchetypeGraph.contains(oldArchetype, columnId))
    {
        return *this;
    }

    auto& oldTable = mWorld.mTables.dense(oldArchetype);

    // Otherwise, we'll need to move the entity to a new archetype.
    auto newArchetype = mWorld.mArchetypeGraph.without(oldArchetype, columnId);
    mWorld.mEntityPool.archetype(mEntity.index, newArchetype);

    // Get the table of the new archetype, which we may have to create.
    auto& newTable = mWorld.mTables.dense(newArchetype, mWorld.mArchetypeGraph, mWorld.mTypes);

    // Move the entity from the old table to the new one.
    oldTable.swapMove(mEntity.index, newTable);

    CUBOS_DEBUG("Removed component {} from entity {}", type.name(), mEntity);
    return *this;
}

World::ConstComponents::ConstComponents(const World& world, Entity entity)
    : mWorld{world}
    , mEntity{entity}
{
    CUBOS_ASSERT(world.isAlive(entity));
}

bool World::ConstComponents::has(const reflection::Type& type) const
{
    auto archetype = mWorld.mEntityPool.archetype(mEntity.index);
    auto columnId = DenseColumnId::make(mWorld.mTypes.id(type));
    return mWorld.mArchetypeGraph.contains(archetype, columnId);
}

const void* World::ConstComponents::get(const reflection::Type& type) const
{
    auto archetype = mWorld.mEntityPool.archetype(mEntity.index);
    const auto& table = mWorld.mTables.dense(archetype);
    auto columnId = DenseColumnId::make(mWorld.mTypes.id(type));
    return table.column(columnId).at(table.row(mEntity.index));
}

auto World::ConstComponents::begin() const -> Iterator
{
    return Iterator{*this, false};
}

auto World::ConstComponents::end() const -> Iterator
{
    return Iterator{*this, true};
}

World::Components::Iterator::Iterator(Components& components, bool end)
    : mComponents{components}
{
    if (end)
    {
        mId = DataTypeId::Invalid;
    }
    else
    {
        auto archetype = mComponents.mWorld.mEntityPool.archetype(mComponents.mEntity.index);
        auto column = mComponents.mWorld.mArchetypeGraph.first(archetype);
        while (column.dataType() != DataTypeId::Invalid && !mComponents.mWorld.mTypes.isComponent(column.dataType()))
        {
            column = mComponents.mWorld.mArchetypeGraph.next(archetype, column);
        }
        mId = column.dataType();
    }
}

bool World::Components::Iterator::operator==(const Iterator& other) const
{
    return mId == other.mId && mComponents.mEntity == other.mComponents.mEntity;
}

auto World::Components::Iterator::operator*() const -> const Component&
{
    CUBOS_ASSERT(mId != DataTypeId::Invalid, "Iterator is out of bounds");
    mComponent.type = &mComponents.mWorld.mTypes.type(mId);
    mComponent.value = mComponents.get(*mComponent.type);
    return mComponent;
}

auto World::Components::Iterator::operator->() const -> const Component*
{
    return &this->operator*();
}

auto World::Components::Iterator::operator++() -> Iterator&
{
    CUBOS_ASSERT(mId != DataTypeId::Invalid, "Iterator is out of bounds");
    auto archetype = mComponents.mWorld.mEntityPool.archetype(mComponents.mEntity.index);
    auto column = mComponents.mWorld.mArchetypeGraph.next(archetype, DenseColumnId::make(mId));
    while (column.dataType() != DataTypeId::Invalid && !mComponents.mWorld.mTypes.isComponent(column.dataType()))
    {
        column = mComponents.mWorld.mArchetypeGraph.next(archetype, column);
    }
    mId = column.dataType();
    return *this;
}

World::ConstComponents::Iterator::Iterator(const ConstComponents& components, bool end)
    : mComponents{components}
{
    if (end)
    {
        mId = DataTypeId::Invalid;
    }
    else
    {
        auto archetype = mComponents.mWorld.mEntityPool.archetype(mComponents.mEntity.index);
        auto column = mComponents.mWorld.mArchetypeGraph.first(archetype);
        while (column.dataType() != DataTypeId::Invalid && !mComponents.mWorld.mTypes.isComponent(column.dataType()))
        {
            column = mComponents.mWorld.mArchetypeGraph.next(archetype, column);
        }
        mId = column.dataType();
    }
}

bool World::ConstComponents::Iterator::operator==(const Iterator& other) const
{
    return mId == other.mId && mComponents.mEntity == other.mComponents.mEntity;
}

auto World::ConstComponents::Iterator::operator*() const -> const Component&
{
    CUBOS_ASSERT(mId != DataTypeId::Invalid, "Iterator is out of bounds");
    mComponent.type = &mComponents.mWorld.mTypes.type(mId);
    mComponent.value = mComponents.get(*mComponent.type);
    return mComponent;
}

auto World::ConstComponents::Iterator::operator->() const -> const Component*
{
    return &this->operator*();
}

auto World::ConstComponents::Iterator::operator++() -> Iterator&
{
    CUBOS_ASSERT(mId != DataTypeId::Invalid, "Iterator is out of bounds");
    auto archetype = mComponents.mWorld.mEntityPool.archetype(mComponents.mEntity.index);
    auto column = mComponents.mWorld.mArchetypeGraph.next(archetype, DenseColumnId::make(mId));
    while (column.dataType() != DataTypeId::Invalid && !mComponents.mWorld.mTypes.isComponent(column.dataType()))
    {
        column = mComponents.mWorld.mArchetypeGraph.next(archetype, column);
    }
    mId = column.dataType();
    return *this;
}
