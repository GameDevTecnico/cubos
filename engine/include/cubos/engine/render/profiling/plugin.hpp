/// @dir
/// @brief @ref render-profiling-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-profiling-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-profiling-plugin RenderProfiling
    /// @ingroup render-plugins
    /// @brief Adds and manages the @ref RenderProfiler resource.
    ///
    /// ## Settings
    /// - `render.profiling` - whether profiling should be enabled (default: `false`).
    ///
    /// ## Resources
    /// - @ref RenderProfiler - holds settings for render performance profiling.
    ///
    /// ## Dependencies
    /// - @ref settings-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-profiling-plugin
    CUBOS_ENGINE_API void renderProfilingPlugin(Cubos& cubos);
} // namespace cubos::engine
