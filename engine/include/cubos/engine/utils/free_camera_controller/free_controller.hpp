/// @file
/// @brief Component @ref cubos::engine::Position.
/// @ingroup transform-plugin

#pragma once

#include <string>

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @brief Component which moves the camera.
    /// @ingroup free-camera-controller-plugin
    struct [[cubos::component("cubos/free_controller")]] FreeController
    {
        CUBOS_REFLECT;

        /// @brief The speed at which the camera moves.
        float speed = 3.0F;

        /// @brief The sensivitiy at which the camera rotates.
        float sens = 5.0F;

        /// @brief The axis of the axe to move the camera vertically.
        float verticalAxis = 0.0F;
        float horizontalZAxis = 0.0F;
        float horizontalXAxis = 0.0F;

        /// @brief The inclination and azimuth of the camera roation.
        float phi = 0.0F;
        float theta = 0.0F;

        glm::ivec2 lastMousePos;
    };
} // namespace cubos::engine
