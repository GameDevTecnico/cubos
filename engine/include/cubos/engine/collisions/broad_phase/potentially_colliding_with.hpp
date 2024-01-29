/// @file
/// @brief Component @ref cubos::engine::PotentiallyCollidingWith.
/// @ingroup collisions-plugin

// FIXME: This should be private, but it's used in the sample.

#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which represents a potential collisions.
    /// @ingroup collisions-plugin
    struct PotentiallyCollidingWith
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
