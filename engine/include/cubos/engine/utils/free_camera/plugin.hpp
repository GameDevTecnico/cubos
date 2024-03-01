/// @dir
/// @brief @ref free-camera-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup free-camera-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/utils/free_camera/controller.hpp>

namespace cubos::engine
{
    /// @defgroup free-camera-plugin Free Camera
    /// @ingroup engine
    /// @brief Adds the free camera controller component, which locks the mouse and moves an entity.
    ///
    /// ## Components
    /// - @ref FreeCameraController - handles logic to move a camera.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup free-camera-plugin
    CUBOS_ENGINE_API void freeCameraPlugin(Cubos& cubos);
} // namespace cubos::engine
