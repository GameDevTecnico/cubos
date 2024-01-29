/// @dir
/// @brief @ref tesseratos-debug-camera-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-debug-camera-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-debug-camera-plugin Debug camera
    /// @ingroup tesseratos
    /// @brief Adds a toggleable debug camera.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref renderer-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-debug-camera-plugin
    void debugCameraPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
