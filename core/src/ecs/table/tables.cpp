#include <cubos/core/ecs/table/tables.hpp>

using cubos::core::ecs::DenseTableRegistry;
using cubos::core::ecs::SparseRelationTableRegistry;
using cubos::core::ecs::Tables;

DenseTableRegistry& Tables::dense()
{
    return mDense;
}

const DenseTableRegistry& Tables::dense() const
{
    return mDense;
}

SparseRelationTableRegistry& Tables::sparseRelation()
{
    return mSparseRelation;
}

const SparseRelationTableRegistry& Tables::sparseRelation() const
{
    return mSparseRelation;
}
