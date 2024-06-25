/// @dir
/// @brief @ref render-shadows-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-shadows-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-shadows-plugin Shadows
    /// @ingroup render-plugins
    /// @brief Base interface plugin for shadows.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-shadows-plugin
    CUBOS_ENGINE_API void shadowsPlugin(Cubos& cubos);
} // namespace cubos::engine
