/// @dir
/// @brief @ref debug-camera-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup debug-camera-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup debug-camera-plugin
    void debugCameraPlugin(Cubos& cubos);
} // namespace cubos::engine::tools
