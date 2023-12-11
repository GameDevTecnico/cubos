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
    struct [[cubos::component("cubos/simple_controller")]] SimpleController
    {
        CUBOS_REFLECT;

        /// @brief The speed at which the camera moves.
        float speed = 1.0F;

        /// @brief The axis of the axe to move the camera vertically.
        float verticalAxis = 0.0f;
        float horizontalZAxis = 0.0f;
        float horizontalXAxis = 0.0f;
    };
} // namespace cubos::engine
