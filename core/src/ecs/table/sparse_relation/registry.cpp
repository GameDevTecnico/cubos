#include <algorithm>

#include <cubos/core/ecs/table/sparse_relation/registry.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
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

void SparseRelationTableRegistry::cleanUp()
{
    std::size_t cleanedUpCount = 0;

    for (auto it = mTables.begin(); it != mTables.end();)
    {
        if (it->second.size() == 0)
        {
            cleanedUpCount += 1;

            // Remove the table from the id array.
            std::size_t i = 0;
            for (; i < mIds.size(); ++i)
            {
                if (mIds[i] == it->first)
                {
                    std::swap(mIds[i], mIds.back());
                    mIds.pop_back();
                    break;
                }
            }

            // Remove the table from the type index.
            mTypeIndices[it->first.dataType].erase(it->first);

            it = mTables.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (cleanedUpCount > 0)
    {
        mVersion += 1;
        CUBOS_DEBUG("Cleaned up {} sparse relation tables", cleanedUpCount);
    }
}

void SparseRelationTableRegistry::TypeIndex::insert(SparseRelationTableId id)
{
    mSparseRelationTableIdsByFrom[id.from].emplace(id);
    mSparseRelationTableIdsByTo[id.to].emplace(id);
    mMaxDepth = mMaxDepth < id.depth ? id.depth : mMaxDepth;
}

void SparseRelationTableRegistry::TypeIndex::erase(SparseRelationTableId id)
{
    mSparseRelationTableIdsByFrom[id.from].erase(id);
    mSparseRelationTableIdsByTo[id.to].erase(id);
}
