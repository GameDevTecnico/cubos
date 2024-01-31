/// @file
/// @brief Struct @ref cubos::core::ecs::SystemAccess.
/// @ingroup core-ecs-system

#pragma once

#include <unordered_set>

#include <cubos/core/ecs/types.hpp>

namespace cubos::core::ecs
{
    /// @brief Describes the types of data a system accesses.
    ///
    /// Used to determine if systems conflict with each other.
    ///
    /// @ingroup core-ecs-system
    struct SystemAccess
    {
        /// @brief Whether the system accesses the world directly.
        bool usesWorld{false};

        /// @brief Set of data types accessed by the system.
        std::unordered_set<DataTypeId, DataTypeIdHash> dataTypes;

        /// @brief Checks if this access patterns intersect with the given ones.
        /// @param other Other system access patterns.
        /// @return Whether they intersect.
        bool intersects(const SystemAccess& other) const;
    };
} // namespace cubos::core::ecs
