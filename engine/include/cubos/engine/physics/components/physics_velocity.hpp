/// @file
/// @brief Component @ref cubos::engine::PhysicsVelocity.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which holds velocity and forces applied on a particle.
    /// @note Should be used with @ref PhysicsMass and @ref Position.
    /// @ingroup physics-plugin
    struct [[cubos::component("cubos/physics_velocity", VecStorage)]] PhysicsVelocity
    {
        CUBOS_REFLECT;

        glm::vec3 velocity;
        glm::vec3 force;
        glm::vec3 impulse;
    };
} // namespace cubos::engine
