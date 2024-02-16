/// @file
/// @brief Component @ref cubos::engine::PhysicsBundle.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which encapsulates the creation all components required for physics.
    /// @ingroup physics-plugin
    struct PhysicsBundle
    {
        CUBOS_REFLECT;

        float mass = 1.0F;
        glm::vec3 velocity = {0.0F, 0.0F, 0.0F};
        glm::vec3 force = {0.0F, 0.0F, 0.0F};
        glm::vec3 impulse = {0.0F, 0.0F, 0.0F};
    };
} // namespace cubos::engine
