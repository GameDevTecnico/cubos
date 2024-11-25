/// @dir
/// @brief @ref render-shadow-casters-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-shadow-casters-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-shadow-casters-plugin Shadow casters
    /// @ingroup render-shadows-plugins
    /// @brief Base interface plugin for shadows.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-shadow-casters-plugin
    CUBOS_ENGINE_API void shadowCastersPlugin(Cubos& cubos);
} // namespace cubos::engine
