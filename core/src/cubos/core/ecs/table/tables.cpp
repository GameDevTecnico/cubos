#include <cubos/core/ecs/table/tables.hpp>

using cubos::core::ecs::DenseTableRegistry;
using cubos::core::ecs::Tables;

DenseTableRegistry& Tables::dense()
{
    return mDense;
}

const DenseTableRegistry& Tables::dense() const
{
    return mDense;
}
