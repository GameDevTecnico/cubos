#include <cubos/core/ecs/table/sparse_relation/registry.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::ecs::SparseRelationTable;
using cubos::core::ecs::SparseRelationTableRegistry;

SparseRelationTableRegistry::~SparseRelationTableRegistry()
{
    delete mEmptyTypeIndex;
}

SparseRelationTableRegistry::SparseRelationTableRegistry()
{
    // A pointer was used here to avoid moving the entire TypeIndex definition inside the registry class.
    // This way we can forward declare it.
    mEmptyTypeIndex = new TypeIndex();
}

void SparseRelationTableRegistry::reset()
{
    mTables.clear();
    mTypeIndices.clear();
    mIds.clear();
}

bool SparseRelationTableRegistry::contains(SparseRelationTableId id) const
{
    return mTables.contains(id);
}

SparseRelationTable& SparseRelationTableRegistry::create(SparseRelationTableId id, Types& types)
{
    if (!mTables.contains(id))
    {
        mTables.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                        std::forward_as_tuple(types.type(id.dataType)));
        mTypeIndices[id.dataType].insert(id);
        mIds.emplace_back(id);
    }

    return mTables.at(id);
}

SparseRelationTable& SparseRelationTableRegistry::at(SparseRelationTableId id)
{
    CUBOS_ASSERT(mTables.contains(id), "No such sparse relation table");
    return mTables.at(id);
}

const SparseRelationTable& SparseRelationTableRegistry::at(SparseRelationTableId id) const
{
    CUBOS_ASSERT(mTables.contains(id), "No such sparse relation table");
    return mTables.at(id);
}

const SparseRelationTableRegistry::TypeIndex& SparseRelationTableRegistry::type(DataTypeId type) const
{
    auto it = mTypeIndices.find(type);
    if (it == mTypeIndices.end())
    {
        return *mEmptyTypeIndex;
    }

    return it->second;
}

void SparseRelationTableRegistry::erase(DataTypeId type)
{
    auto it = mTypeIndices.find(type);
    if (it != mTypeIndices.end())
    {
        for (auto [archetype, tables] : it->second.from())
        {
            for (auto id : tables)
            {
                mTables.at(id).clear();
            }
        }

        for (auto [archetype, tables] : it->second.to())
        {
            for (auto id : tables)
            {
                mTables.at(id).clear();
            }
        }
    }
}

void SparseRelationTableRegistry::TypeIndex::insert(SparseRelationTableId id)
{
    mSparseRelationTableIdsByFrom[id.from].emplace_back(id);
    mSparseRelationTableIdsByTo[id.to].emplace_back(id);
    mMaxDepth = mMaxDepth < id.depth ? id.depth : mMaxDepth;
}
