/// @file
/// @brief Component @ref cubos::engine::PhysicsVelocity.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

namespace cubos::engine
{
    /// @brief Component which defines parameters of a particle.
    /// @note Should be used with @ref PhysicsMass and @ref Position.
    /// @ingroup physics-plugin
    struct [[cubos::component("cubos/physics_velocity", VecStorage)]] PhysicsVelocity
    {
        glm::vec3 velocity;
        glm::vec3 acceleration;
        glm::vec3 totalForce;
        glm::vec3 gravity;
        float damping;

        bool added;
    };
} // namespace cubos::engine
