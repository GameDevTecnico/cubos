/// @dir
/// @brief @ref gizmos-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup gizmos-plugin

#pragma once

#include <cubos/engine/gizmos/gizmos.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup gizmos-plugin Gizmos
    /// @ingroup engine
    /// @brief Used to draw gizmos helpful for debugging and tools.
    ///
    /// ## Resources
    /// - @ref Gizmos - used to queue gizmo draw commands.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref transform-plugin
    /// - @ref render-picker-plugin
    /// - @ref render-camera-plugin
    /// - @ref render-target-plugin

    /// @brief Gizmos renderer is initialized, after @ref windowInitTag.
    CUBOS_ENGINE_API extern Tag gizmosInitTag;

    /// @brief Gizmos interaction is handled, after @ref windowPollTag and before @ref gizmosDrawTag.
    CUBOS_ENGINE_API extern Tag gizmosInputTag;

    /// @brief Gizmos are drawn to all render targets with @ref GizmosTarget components.
    CUBOS_ENGINE_API extern Tag gizmosDrawTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup gizmos-plugin
    CUBOS_ENGINE_API void gizmosPlugin(Cubos& cubos);
} // namespace cubos::engine
