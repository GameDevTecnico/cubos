#include <cubos/core/ecs/table/tables.hpp>
#include <cubos/core/log.hpp>

using cubos::core::ecs::DenseTable;
using cubos::core::ecs::Tables;

Tables::Tables()
{
    mDenseTables.emplace(std::piecewise_construct, std::forward_as_tuple(0), std::forward_as_tuple());
}

bool Tables::contains(ArchetypeId archetype) const
{
    return mDenseTables.contains(archetype.inner);
}

DenseTable& Tables::dense(ArchetypeId archetype, ArchetypeGraph& graph, Types& types)
{
    if (mDenseTables.contains(archetype.inner))
    {
        return mDenseTables.at(archetype.inner);
    }

    auto& table = mDenseTables[archetype.inner];

    // Add each dense column associated to the archetype to the table.
    for (auto id = graph.first(archetype); id != DenseColumnId::Invalid; id = graph.next(archetype, id))
    {
        table.addColumn(id, types.type(id.dataType()));
    }

    return table;
}

DenseTable& Tables::dense(ArchetypeId archetype)
{
    CUBOS_ASSERT(mDenseTables.contains(archetype.inner), "Archetype does not have a dense table");
    return mDenseTables.at(archetype.inner);
}

const DenseTable& Tables::dense(ArchetypeId archetype) const
{
    CUBOS_ASSERT(mDenseTables.contains(archetype.inner), "Archetype does not have a dense table");
    return mDenseTables.at(archetype.inner);
}
