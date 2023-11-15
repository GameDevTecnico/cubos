/// @dir
/// @brief @ref free-camera-controller-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup free-camera-controller-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/utils/free_camera_controller/free_camera_controller.hpp>

/// @brief TEMPORARY SUBJECT TO CHANGE!!
void updateVertical(float vertical, cubos::core::ecs::Write<cubos::engine::FreeCameraController> controller);
void updateHorizontalX(float horizontalX, cubos::core::ecs::Write<cubos::engine::FreeCameraController> controller);
void updateHorizontalY(float horizontalY, cubos::core::ecs::Write<cubos::engine::FreeCameraController> controller);

namespace cubos::engine
{
    /// @defgroup free-camera-controller-plugin FreeCameraController
    /// @ingroup engine
    /// @brief Adds a free camera controller to the scene that accepts input and moves a camera accordingly.
    ///
    /// This plugin operates on entities with a @ref Camera component.
    ///
    /// @note Any camera gets automatically a @ref FreeCameraController component.
    ///
    /// ## Components
    /// - @ref FreeCameraController - handles logic to move a camera. 
    ///
    /// ## Tags
    /// - ???

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup free-camera-controller-plugin-plugin
    void freeCameraControllerPlugin(Cubos& cubos);
} // namespace cubos::engine
