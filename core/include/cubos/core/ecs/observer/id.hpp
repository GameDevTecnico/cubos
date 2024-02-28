/// @file
/// @brief Struct @ref cubos::core::ecs::ObserverId.
/// @ingroup core-ecs-observer

#pragma once

#include <cstddef>

namespace cubos::core::ecs
{
    /// @brief Identifies an observer.
    /// @ingroup core-ecs-observer
    struct ObserverId
    {
        std::size_t inner; ///< Observer identifier.

        /// @brief Compares two observer identifiers for equality.
        /// @param other Other observer identifier.
        /// @return Whether the two observer identifiers are equal.
        bool operator==(const ObserverId& other) const = default;
    };
} // namespace cubos::core::ecs
