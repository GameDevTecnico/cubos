/// @file
/// @brief Component @ref cubos::engine::Position.
/// @ingroup transform-plugin

#pragma once

#include <string>

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @brief Component which moves the camera.
    /// @ingroup free-camera-controller-plugin
    struct SimpleController
    {
        CUBOS_REFLECT;

        /// @brief The speed at which the camera moves.
        float speed = 1.0F;

        /// @brief The axis of the axe to move the camera vertically.
        float verticalAxis = 0.0F;
        float horizontalZAxis = 0.0F;
        float horizontalXAxis = 0.0F;
    };
} // namespace cubos::engine
