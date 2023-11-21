/// @file
/// @brief Component @ref cubos::engine::Position.
/// @ingroup transform-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <glm/vec2.hpp>
#include <string>

namespace cubos::engine
{
    /// @brief Component which moves the camera.
    /// @ingroup free-camera-controller-plugin
    struct [[cubos::component("cubos/free_controller")]] FreeController
    {
        CUBOS_REFLECT;

        /// @brief The speed at which the camera moves.
        float speed = 1.0f;

        /// @brief The sensivitiy at which the camera rotates.
        float sens = 1.0f;

        /// @brief The axis of the axe to move the camera vertically.
        float verticalAxis = 0.0f;
        float horizontalZAxis = 0.0f;
        float horizontalXAxis = 0.0f;

        /// @brief The inclination and azimuth of the camera roation.
        float phi = 0.0f;
        float theta = 0.0f;
    };
} // namespace cubos::engine
