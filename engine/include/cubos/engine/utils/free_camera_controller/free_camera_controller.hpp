/// @file
/// @brief Component @ref cubos::engine::Position.
/// @ingroup transform-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/transform/position.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <glm/vec3.hpp>
#include <string>

namespace cubos::engine
{
    /// @brief Component which moves the camera.
    /// @ingroup free-camera-controller-plugin
    struct [[cubos::component("cubos/free_camera_controller")]] FreeCameraController
    {
        CUBOS_REFLECT;

        /// @brief The speed at which the camera moves.
        float speed = 0.01f;

        /// @brief The axis of the axe to move the camera vertically.
        float verticalAxis = 0.0f;
        float horizontalYAxis = 0.0f;
        float horizontalXAxis = 0.0f;
    };
} // namespace cubos::engine
