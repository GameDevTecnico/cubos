/// @dir
/// @brief @ref debug-camera-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup debug-camera-tool-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup debug-camera-tool-plugin Debug Camera
    /// @ingroup tool-plugins
    /// @brief Adds a toggleable debug camera.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup debug-camera-tool-plugin
    CUBOS_ENGINE_API void debugCameraPlugin(Cubos& cubos);
} // namespace cubos::engine
