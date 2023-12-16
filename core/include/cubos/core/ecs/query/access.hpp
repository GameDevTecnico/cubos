/// @file
/// @brief Struct @ref cubos::core::ecs::QueryAccess.
/// @ingroup core-ecs-query

#pragma once

#include <unordered_set>

#include <cubos/core/ecs/types.hpp>

namespace cubos::core::ecs
{
    /// @brief Describes the types of data a query accesses.
    ///
    /// Used to determine if systems conflict with each other.
    ///
    /// @ingroup core-ecs-query
    struct QueryAccess
    {
        std::unordered_set<uint32_t> dataTypes;

        /// @brief Adds a type identifier to the set of types the query accesses.
        /// @param id Type identifier.
        void insert(DataTypeId id);

        /// @brief Checks if this access patterns intersect with the given ones.
        /// @param other Other query access patterns.
        /// @return Whether they intersect.
        bool intersects(const QueryAccess& other) const;
    };
} // namespace cubos::core::ecs
