/// @file
/// @brief Struct @ref cubos::core::ecs::ArchetypeId.
/// @ingroup core-ecs-entity

#pragma once

#include <cstddef>

#include <cubos/core/api.hpp>

namespace cubos::core::ecs
{
    /// @brief Identifies an archetype.
    /// @ingroup core-ecs-entity
    struct CUBOS_CORE_API ArchetypeId
    {
        std::size_t inner; ///< Archetype identifier.

        static const ArchetypeId Empty;   ///< Empty archetype identifier.
        static const ArchetypeId Invalid; ///< Invalid archetype identifier.

        /// @brief Compares two archetype identifiers for equality.
        /// @param other Other archetype identifier.
        /// @return Whether the two archetype identifiers are equal.
        bool operator==(const ArchetypeId& other) const = default;
    };

    /// @brief Hash functor for @ref ArchetypeId.
    struct CUBOS_CORE_API ArchetypeIdHash
    {
        std::size_t operator()(const ArchetypeId& id) const;
    };
} // namespace cubos::core::ecs
