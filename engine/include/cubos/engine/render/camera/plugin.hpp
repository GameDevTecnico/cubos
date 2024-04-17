/// @dir
/// @brief @ref render-camera-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-camera-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-camera-plugin Camera
    /// @ingroup render-plugins
    /// @brief Adds @ref PerspectiveCamera components and the @ref DrawsTo relation.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-camera-plugin
    void cameraPlugin(Cubos& cubos);
} // namespace cubos::engine
