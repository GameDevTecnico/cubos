#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

World::World()
{
    this->registerComponent<Name>();
}

void World::registerComponent(const reflection::Type& type)
{
    CUBOS_TRACE("Registered component '{}'", type.name());
    mTypes.addComponent(type);
}

void World::registerRelation(const reflection::Type& type)
{
    CUBOS_TRACE("Registered relation '{}'", type.name());
    mTypes.addRelation(type);
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
    CUBOS_ASSERT(mEntityPool.archetype(entity.index) == ArchetypeId::Invalid, "Entity has already been created");
    mEntityPool.archetype(entity.index, ArchetypeId::Empty);

    // Push the entity to the archetype's table.
    mTables.dense().at(ArchetypeId::Empty).pushBack(entity.index);
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

    auto archetype = mEntityPool.archetype(entity.index);
    mEntityPool.destroy(entity.index);
    mTables.dense().at(archetype).swapErase(entity.index);

    for (const auto& [_, index] : mTables.sparseRelation())
    {
        // For each table where the entity's archetype is the 'from' archetype.
        if (index.from().contains(archetype))
        {
            for (const auto& table : index.from().at(archetype))
            {
                // Erase all occurrences of the entity in the 'from' column.
                mTables.sparseRelation().at(table).eraseFrom(entity.index);
            }
        }

        // For each table where the entity's archetype is the 'to' archetype.
        if (index.to().contains(archetype))
        {
            for (const auto& table : index.to().at(archetype))
            {
                // Erase all occurrences of the entity in the 'to' column.
                mTables.sparseRelation().at(table).eraseTo(entity.index);
            }
        }
    }

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

void World::relate(Entity from, Entity to, const reflection::Type& type, void* value)
{
    CUBOS_ASSERT(this->isAlive(from));
    CUBOS_ASSERT(this->isAlive(to));

    auto dataType = mTypes.id(type);
    CUBOS_ASSERT(mTypes.isRelation(dataType), "Type {} is not registered as a relation", type.name());

    if (mTypes.isSymmetricRelation(dataType))
    {
        // If the relation is symmetric, then we need to make sure that the 'from' entity is always
        // the one with the lowest index.
        if (from.index > to.index)
        {
            std::swap(from, to);
        }
    }

    // Create a sparse relation table identifier from the archetypes of both entities.
    auto fromArchetype = mEntityPool.archetype(from.index);
    auto toArchetype = mEntityPool.archetype(to.index);
    auto tableId = SparseRelationTableId{dataType, fromArchetype, toArchetype};

    if (mTypes.isTreeRelation(dataType))
    {
        // TODO: this is a bit expensive at the moment, since we have to iterate over all tables with
        // the same from archetype. Maybe we should keep an index to find the tables directly from the entities.
        const auto& fromTables = mTables.sparseRelation().type(dataType).from();

        // If the relation is tree-like, then we need to find the depth of the 'to' entity in the tree,
        // to figure out which table to insert this relation into.
        if (fromTables.contains(toArchetype))
        {
            for (auto otherTableId : fromTables.at(toArchetype))
            {
                // Check if the 'to' entity is on this table.
                auto& table = mTables.sparseRelation().at(otherTableId);
                auto row = table.firstFrom(to.index);
                if (row != table.size())
                {
                    // It is, so the depth of the new relation is one more than the depth of the 'to' entity.
                    tableId.depth = otherTableId.depth + 1;
                    break;
                }
            }
        }

        // There can only be one outgoing relation from the 'from' entity, so we also need to erase
        // any existing relation, if there is one.
        if (fromTables.contains(fromArchetype))
        {
            for (auto tableId : fromTables.at(fromArchetype))
            {
                // Check if the 'from' entity is on this table.
                auto& table = mTables.sparseRelation().at(tableId);
                auto row = table.firstFrom(from.index);
                if (row != table.size())
                {
                    // If it is, then call unrelate() to erase the relation.
                    uint32_t fromIndex;
                    uint32_t toIndex;
                    table.indices(row, fromIndex, toIndex);
                    this->unrelate(from, Entity{toIndex, this->generation(toIndex)}, type);
                    break;
                }
            }
        }

        // We need to update the depth of the incoming relation.
        this->propagateDepth(from.index, dataType, tableId.depth + 1);
    }

    // Get or create the table and insert a new row.
    auto& table = mTables.sparseRelation().create(tableId, mTypes);
    table.insert(from.index, to.index, value);
    CUBOS_TRACE("Added relation {} from entity {} to entity {}", type.name(), from, to);
}

void World::unrelate(Entity from, Entity to, const reflection::Type& type)
{
    CUBOS_ASSERT(this->isAlive(from));
    CUBOS_ASSERT(this->isAlive(to));

    auto dataType = mTypes.id(type);
    CUBOS_ASSERT(mTypes.isRelation(dataType), "Type {} is not registered as a relation", type.name());

    if (mTypes.isSymmetricRelation(dataType))
    {
        // If the relation is symmetric, then we need to make sure that the 'from' entity is always
        // the one with the lowest index.
        if (from.index > to.index)
        {
            std::swap(from, to);
        }
    }

    auto fromArchetype = mEntityPool.archetype(from.index);
    auto toArchetype = mEntityPool.archetype(to.index);

    // Search for the table with the right depth which contains the relation.
    for (int depth = 0, maxDepth = mTables.sparseRelation().type(dataType).maxDepth(); depth <= maxDepth; ++depth)
    {
        auto tableId = SparseRelationTableId{dataType, fromArchetype, toArchetype, depth};
        if (mTables.sparseRelation().contains(tableId))
        {
            auto& table = mTables.sparseRelation().at(tableId);
            if (table.erase(from.index, to.index))
            {
                CUBOS_TRACE("Removed relation {} from entity {} to entity {}", type.name(), from, to);

                if (mTypes.isTreeRelation(dataType))
                {
                    // If the relation is tree-like, then we are changing the depth of the 'from' entity to 0.
                    // We must update any incoming relations to the 'from' entity to reflect this.
                    this->propagateDepth(from.index, dataType, 0);
                }

                break;
            }
        }
    }
}

bool World::related(Entity from, Entity to, const reflection::Type& type) const
{
    CUBOS_ASSERT(this->isAlive(from));
    CUBOS_ASSERT(this->isAlive(to));

    auto dataType = mTypes.id(type);
    CUBOS_ASSERT(mTypes.isRelation(dataType));

    if (mTypes.isSymmetricRelation(dataType))
    {
        // If the relation is symmetric, then we need to make sure that the 'from' entity is always
        // the one with the lowest index.
        if (from.index > to.index)
        {
            std::swap(from, to);
        }
    }

    auto fromArchetype = mEntityPool.archetype(from.index);
    auto toArchetype = mEntityPool.archetype(to.index);

    // Check if there's a table with the given archetypes and any depth which contains the relation.
    for (int depth = 0, maxDepth = mTables.sparseRelation().type(dataType).maxDepth(); depth <= maxDepth; ++depth)
    {
        auto tableId = SparseRelationTableId{dataType, fromArchetype, toArchetype, depth};
        if (mTables.sparseRelation().contains(tableId) &&
            mTables.sparseRelation().at(tableId).contains(from.index, to.index))
        {
            return true;
        }
    }

    return false;
}

void* World::relation(Entity from, Entity to, const reflection::Type& type)
{
    CUBOS_ASSERT(this->isAlive(from));
    CUBOS_ASSERT(this->isAlive(to));

    auto dataType = mTypes.id(type);
    CUBOS_ASSERT(mTypes.isRelation(dataType));

    if (mTypes.isSymmetricRelation(dataType))
    {
        // If the relation is symmetric, then we need to make sure that the 'from' entity is always
        // the one with the lowest index.
        if (from.index > to.index)
        {
            std::swap(from, to);
        }
    }

    auto fromArchetype = mEntityPool.archetype(from.index);
    auto toArchetype = mEntityPool.archetype(to.index);

    // Check if there's a table with the given archetypes and any depth which contains the relation.
    for (int depth = 0, maxDepth = mTables.sparseRelation().type(dataType).maxDepth(); depth <= maxDepth; ++depth)
    {
        auto tableId = SparseRelationTableId{dataType, fromArchetype, toArchetype, depth};
        if (mTables.sparseRelation().contains(tableId) &&
            mTables.sparseRelation().at(tableId).contains(from.index, to.index))
        {
            auto row = mTables.sparseRelation().at(tableId).row(from.index, to.index);
            return mTables.sparseRelation().at(tableId).at(row);
        }
    }

    CUBOS_FAIL("No such relation {} from {} to {}", type.name(), from, to);
}

const void* World::relation(Entity from, Entity to, const reflection::Type& type) const
{
    CUBOS_ASSERT(this->isAlive(from));
    CUBOS_ASSERT(this->isAlive(to));

    auto dataType = mTypes.id(type);
    CUBOS_ASSERT(mTypes.isRelation(dataType));

    if (mTypes.isSymmetricRelation(dataType))
    {
        // If the relation is symmetric, then we need to make sure that the 'from' entity is always
        // the one with the lowest index.
        if (from.index > to.index)
        {
            std::swap(from, to);
        }
    }

    auto fromArchetype = mEntityPool.archetype(from.index);
    auto toArchetype = mEntityPool.archetype(to.index);

    // Check if there's a table with the given archetypes and any depth which contains the relation.
    for (int depth = 0, maxDepth = mTables.sparseRelation().type(dataType).maxDepth(); depth <= maxDepth; ++depth)
    {
        auto tableId = SparseRelationTableId{dataType, fromArchetype, toArchetype, depth};
        if (mTables.sparseRelation().contains(tableId) &&
            mTables.sparseRelation().at(tableId).contains(from.index, to.index))
        {
            auto row = mTables.sparseRelation().at(tableId).row(from.index, to.index);
            return mTables.sparseRelation().at(tableId).at(row);
        }
    }

    CUBOS_FAIL("No such relation {} from {} to {}", type.name(), from, to);
}

void World::propagateDepth(uint32_t index, DataTypeId dataType, int depth)
{
    auto archetype = mEntityPool.archetype(index);
    const auto& toTables = mTables.sparseRelation().type(dataType).to();
    if (toTables.contains(archetype))
    {
        // Collect all tables which contain relations which have the entity as the 'to' entity.
        std::vector<SparseRelationTableId> tables{};
        for (const auto& tableId : toTables.at(archetype))
        {
            auto& table = mTables.sparseRelation().at(tableId);
            auto row = table.firstTo(index);
            if (row != table.size())
            {
                tables.push_back(tableId);
            }
        }

        // Iterate over these tables and move all relations with the entity to the new table.
        for (const auto& tableId : tables)
        {
            // Why would we ever call this function without actually changing the depth?
            CUBOS_ASSERT(tableId.depth != depth);

            // Before moving the relations, recurse into the 'from' entities as their depth will change.
            auto& table = mTables.sparseRelation().at(tableId);
            for (auto row = table.firstTo(index); row != table.size(); row = table.nextTo(row))
            {
                // Get the 'from' entity and recurse.
                uint32_t fromIndex;
                uint32_t toIndex;
                table.indices(row, fromIndex, toIndex);
                this->propagateDepth(fromIndex, dataType, depth + 1);
            }

            // Move all incoming relations to the new table.
            auto newTableId = tableId;
            newTableId.depth = depth;
            auto& newTable = mTables.sparseRelation().create(newTableId, mTypes);
            table.moveTo(index, newTable);
        }
    }
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
    auto columnId = ColumnId::make(mWorld.mTypes.id(type));
    return mWorld.mArchetypeGraph.contains(archetype, columnId);
}

void* World::Components::get(const reflection::Type& type)
{
    auto archetype = mWorld.mEntityPool.archetype(mEntity.index);
    auto& table = mWorld.mTables.dense().at(archetype);
    auto columnId = ColumnId::make(mWorld.mTypes.id(type));
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
    CUBOS_ASSERT(mWorld.mTypes.isComponent(typeId), "Type {} is not registered as a component", type.name());
    auto columnId = ColumnId::make(typeId);

    auto oldArchetype = mWorld.mEntityPool.archetype(mEntity.index);
    auto& oldTable = mWorld.mTables.dense().at(oldArchetype);

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
    auto& newTable = mWorld.mTables.dense().create(newArchetype, mWorld.mArchetypeGraph, mWorld.mTypes);

    // Move the entity from the old table to the new one and add the new component.
    oldTable.swapMove(mEntity.index, newTable);
    newTable.column(columnId).pushMove(value);

    // For each sparse relation table the entity is on, move its relations to the new corresponding one.
    for (const auto& [_, index] : mWorld.mTables.sparseRelation())
    {
        // For each table where the entity's archetype is the 'from' archetype.
        if (index.from().contains(oldArchetype))
        {
            for (const auto& oldTableId : index.from().at(oldArchetype))
            {
                // Move all occurrences of the entity in the 'from' column to the new table.
                auto newTableId = oldTableId;
                newTableId.from = newArchetype;
                auto& newTable = mWorld.mTables.sparseRelation().create(newTableId, mWorld.mTypes);
                mWorld.mTables.sparseRelation().at(oldTableId).moveFrom(mEntity.index, newTable);
            }
        }

        // For each table where the entity's archetype is the 'to' archetype.
        if (index.to().contains(oldArchetype))
        {
            for (const auto& oldTableId : index.to().at(oldArchetype))
            {
                // Move all occurrences of the entity in the 'to' column to the new table.
                auto newTableId = oldTableId;
                newTableId.to = newArchetype;
                auto& newTable = mWorld.mTables.sparseRelation().create(newTableId, mWorld.mTypes);
                mWorld.mTables.sparseRelation().at(oldTableId).moveTo(mEntity.index, newTable);
            }
        }
    }

    CUBOS_TRACE("Added component {} to entity {}", type.name(), mEntity, mEntity);
    return *this;
}

auto World::Components::remove(const reflection::Type& type) -> Components&
{
    auto typeId = mWorld.mTypes.id(type);
    CUBOS_ASSERT(mWorld.mTypes.isComponent(typeId), "Type {} is not registered as a component", type.name());
    auto columnId = ColumnId::make(typeId);

    // If the old archetype doesn't contain this component type, then we don't do anything.
    auto oldArchetype = mWorld.mEntityPool.archetype(mEntity.index);
    if (!mWorld.mArchetypeGraph.contains(oldArchetype, columnId))
    {
        return *this;
    }

    auto& oldTable = mWorld.mTables.dense().at(oldArchetype);

    // Otherwise, we'll need to move the entity to a new archetype.
    auto newArchetype = mWorld.mArchetypeGraph.without(oldArchetype, columnId);
    mWorld.mEntityPool.archetype(mEntity.index, newArchetype);

    // Get the table of the new archetype, which we may have to create.
    auto& newTable = mWorld.mTables.dense().create(newArchetype, mWorld.mArchetypeGraph, mWorld.mTypes);

    // Move the entity from the old table to the new one.
    oldTable.swapMove(mEntity.index, newTable);

    // For each sparse relation table the entity is on, move its relations to the new corresponding one.
    for (const auto& [_, index] : mWorld.mTables.sparseRelation())
    {
        // For each table where the entity's archetype is the 'from' archetype.
        if (index.from().contains(oldArchetype))
        {
            for (const auto& oldTableId : index.from().at(oldArchetype))
            {
                // Move all occurrences of the entity in the 'from' column to the new table.
                auto newTableId = oldTableId;
                newTableId.from = newArchetype;
                auto& newTable = mWorld.mTables.sparseRelation().create(newTableId, mWorld.mTypes);
                mWorld.mTables.sparseRelation().at(oldTableId).moveFrom(mEntity.index, newTable);
            }
        }

        // For each table where the entity's archetype is the 'to' archetype.
        if (index.to().contains(oldArchetype))
        {
            for (const auto& oldTableId : index.to().at(oldArchetype))
            {
                // Move all occurrences of the entity in the 'to' column to the new table.
                auto newTableId = oldTableId;
                newTableId.to = newArchetype;
                auto& newTable = mWorld.mTables.sparseRelation().create(newTableId, mWorld.mTypes);
                mWorld.mTables.sparseRelation().at(oldTableId).moveTo(mEntity.index, newTable);
            }
        }
    }

    CUBOS_TRACE("Removed component {} from entity {}", type.name(), mEntity);
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
    auto columnId = ColumnId::make(mWorld.mTypes.id(type));
    return mWorld.mArchetypeGraph.contains(archetype, columnId);
}

const void* World::ConstComponents::get(const reflection::Type& type) const
{
    auto archetype = mWorld.mEntityPool.archetype(mEntity.index);
    const auto& table = mWorld.mTables.dense().at(archetype);
    auto columnId = ColumnId::make(mWorld.mTypes.id(type));
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
    auto column = mComponents.mWorld.mArchetypeGraph.next(archetype, ColumnId::make(mId));
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
    auto column = mComponents.mWorld.mArchetypeGraph.next(archetype, ColumnId::make(mId));
    while (column.dataType() != DataTypeId::Invalid && !mComponents.mWorld.mTypes.isComponent(column.dataType()))
    {
        column = mComponents.mWorld.mArchetypeGraph.next(archetype, column);
    }
    mId = column.dataType();
    return *this;
}
