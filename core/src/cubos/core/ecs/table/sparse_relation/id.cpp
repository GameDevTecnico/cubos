#include <cstdint>

#include <cubos/core/ecs/table/sparse_relation/id.hpp>

using cubos::core::ecs::SparseRelationTableId;
using cubos::core::ecs::SparseRelationTableIdHash;

SparseRelationTableId::SparseRelationTableId(DataTypeId dataType, ArchetypeId from, ArchetypeId to, int depth)
    : dataType(dataType)
    , from(from)
    , to(to)
    , depth(depth)
{
}

std::size_t SparseRelationTableIdHash::operator()(const SparseRelationTableId& id) const
{
    return static_cast<std::size_t>(id.dataType.inner) ^ id.from.inner ^ id.to.inner ^
           static_cast<std::size_t>(id.depth);
}
