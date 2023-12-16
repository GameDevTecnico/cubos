/// @file
/// @brief Class @ref cubos::core::ecs::QueryProperties.
/// @ingroup core-ecs-query

#pragma once

#include <cubos/core/ecs/table/column.hpp>

namespace cubos::core::ecs
{
    /// @brief Describes a query and what entities it should match.
    struct QueryProperties
    {
        /// @brief Columns required for an entity to match the query.
        std::vector<DenseColumnId> withColumns;

        /// @brief Columns which must not be present on an entity for it to match the query.
        std::vector<DenseColumnId> withoutColumns;
    };
} // namespace cubos::core::ecs
