/// @dir
/// @brief @ref render-lights-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-lights-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-lights-plugin Lights
    /// @ingroup render-plugins
    /// @brief Common interface for the lighting part of rendering plugins.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-lights-plugin
    CUBOS_ENGINE_API void lightsPlugin(Cubos& cubos);
} // namespace cubos::engine
