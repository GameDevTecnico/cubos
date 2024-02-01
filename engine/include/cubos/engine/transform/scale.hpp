/// @file
/// @brief Component @ref cubos::engine::Scale.
/// @ingroup transform-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which assigns a uniform scale to an entity.
    /// @sa LocalToWorld Holds the resulting transform matrix.
    /// @ingroup transform-plugin
    struct Scale
    {
        CUBOS_REFLECT;

        float factor{1.0F}; ///< Uniform scale factor of the entity.
    };
} // namespace cubos::engine
