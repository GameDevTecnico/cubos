#include <cubos/core/ecs/table/dense/registry.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::ecs::DenseTable;
using cubos::core::ecs::DenseTableRegistry;

DenseTableRegistry::DenseTableRegistry()
{
    mTables.emplace(std::piecewise_construct, std::forward_as_tuple(0), std::forward_as_tuple());
}

void DenseTableRegistry::reset()
{
    mTables.clear();
    mTables.emplace(std::piecewise_construct, std::forward_as_tuple(0), std::forward_as_tuple());
}

bool DenseTableRegistry::contains(ArchetypeId archetype) const
{
    return mTables.contains(archetype.inner);
}

DenseTable& DenseTableRegistry::create(ArchetypeId archetype, ArchetypeGraph& graph, Types& types)
{
    if (mTables.contains(archetype.inner))
    {
        return mTables.at(archetype.inner);
    }

    auto& table = mTables[archetype.inner];

    // Add each dense column associated to the archetype to the table.
    for (auto id = graph.first(archetype); id != ColumnId::Invalid; id = graph.next(archetype, id))
    {
        table.addColumn(id, types.type(id.dataType()));
    }

    return table;
}

DenseTable& DenseTableRegistry::at(ArchetypeId archetype)
{
    CUBOS_ASSERT(mTables.contains(archetype.inner), "Archetype does not have a dense table");
    return mTables.at(archetype.inner);
}

const DenseTable& DenseTableRegistry::at(ArchetypeId archetype) const
{
    CUBOS_ASSERT(mTables.contains(archetype.inner), "Archetype does not have a dense table");
    return mTables.at(archetype.inner);
}
