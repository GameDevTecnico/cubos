/// @file
/// @brief Class @ref cubos::core::ecs::Tables.
/// @ingroup core-ecs-table

#pragma once

#include <cubos/core/ecs/table/dense/registry.hpp>
#include <cubos/core/ecs/table/sparse_relation/registry.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores the tables of a given world.
    /// @ingroup core-ecs-table
    class Tables final
    {
    public:
        /// @brief Gets a reference to the dense table registry.
        /// @return Reference to dense table registry.
        DenseTableRegistry& dense();

        /// @copydoc dense()
        const DenseTableRegistry& dense() const;

        /// @brief Gets a reference to the sparse relation table registry.
        /// @return Reference to sparse relation table registry.
        SparseRelationTableRegistry& sparseRelation();

        /// @copydoc sparseRelation()
        const SparseRelationTableRegistry& sparseRelation() const;

    private:
        DenseTableRegistry mDense;
        SparseRelationTableRegistry mSparseRelation;
    };
} // namespace cubos::core::ecs
