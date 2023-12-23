/// @file
/// @brief Component @ref cubos::engine::FreeCameraController.
/// @ingroup free-camera-controller-plugin

#pragma once

#include <string>

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>


namespace cubos::engine
{
    /// @brief Component which moves the camera.
    /// @ingroup free-camera-controller-plugin
    struct FreeCameraController
    {
        CUBOS_REFLECT;

        /// @brief Speed at which the camera moves.
        float speed = 3.0F;

        /// @brief Sensitivity of the camera rotation, i.e,, how fast the camera rotates.
        float sens = 5.0F;

        /// @brief The axis of the axe to move the camera vertically.
        float verticalAxis = 0.0F;
        float horizontalZAxis = 0.0F;
        float horizontalXAxis = 0.0F;

        /// @brief Inclination of the camera.
        float phi = 0.0F;
        
        /// @brief Azimuth of the camera.
        float theta = 0.0F;

        glm::ivec2 lastMousePos;
    };
} // namespace cubos::engine
