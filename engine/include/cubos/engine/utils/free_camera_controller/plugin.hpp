/// @dir
/// @brief @ref free-camera-controller-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup free-camera-controller-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/utils/free_camera_controller/simple_controller.hpp>
#include <cubos/engine/utils/free_camera_controller/free_controller.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/transform/rotation.hpp>
#include <cubos/core/ecs/system/event/reader.hpp>
#include <cubos/engine/window/plugin.hpp>

/// @brief TEMPORARY SUBJECT TO CHANGE!!
void updateSimpleVertical(float vertical, cubos::core::ecs::Write<cubos::engine::SimpleController> controller);
void updateSimpleHorizontalX(float horizontalX, cubos::core::ecs::Write<cubos::engine::SimpleController> controller);
void updateSimpleHorizontalZ(float horizontalY, cubos::core::ecs::Write<cubos::engine::SimpleController> controller);

/// @brief TEMPORARY SUBJECT TO CHANGE!!
void updateFreeVertical(float vertical, cubos::core::ecs::Write<cubos::engine::FreeController> controller);
void updateFreeHorizontalX(float horizontalX, cubos::core::ecs::Write<cubos::engine::FreeController> controller);
void updateFreeHorizontalZ(float horizontalY, cubos::core::ecs::Write<cubos::engine::FreeController> controller);


namespace cubos::engine
{
    /// @defgroup free-camera-controller-plugin FreeCameraController
    /// @ingroup engine
    /// @brief Adds a free camera controller to the scene that accepts input and moves a camera accordingly.
    ///
    /// This plugin operates on entities with a @ref Camera component.
    ///
    /// @note Any camera gets automatically a @ref SimpleController component.
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
