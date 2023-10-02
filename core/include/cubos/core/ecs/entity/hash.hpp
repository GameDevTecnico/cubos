/// @file
/// @brief Struct @ref cubos::core::ecs::EntityHash.
/// @ingroup core-ecs

#pragma once

#include <cstddef>

namespace cubos::core::ecs
{
    struct Entity;

    /// @brief Used to hash @ref Entity objects.
    ///
    /// Can be used to allow @ref Entity objects to be used as keys in an `std::unordered_map`.
    ///
    /// @ingroup core-ecs
    struct EntityHash
    {
        std::size_t operator()(const Entity& entity) const noexcept;
    };
} // namespace cubos::core::ecs
