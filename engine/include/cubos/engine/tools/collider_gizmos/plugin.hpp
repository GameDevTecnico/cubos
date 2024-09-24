/// @dir
/// @brief @ref collider-gizmos-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup collider-gizmos-tool-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup collider-gizmos-tool-plugin Collider Gizmos
    /// @ingroup tool-plugins
    /// @brief Draws gizmos for colliders of selected entities.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup collider-gizmos-tool-plugin
    CUBOS_ENGINE_API void colliderGizmosPlugin(Cubos& cubos);
} // namespace cubos::engine
