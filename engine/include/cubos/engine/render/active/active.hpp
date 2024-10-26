/// @file
/// @brief Component @ref cubos::engine::Active
/// @ingroup render-active-plugin
#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the active state for an entity.
    /// @note Added automatically once a specific camera or light is added.
    /// @ingroup render-active-plugin
    struct Active
    {
        CUBOS_REFLECT;

        /// @brief Whether the entity is active.
        bool active = true;
    };
} // namespace cubos::engine