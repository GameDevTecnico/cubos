/// @file
/// @brief Component @ref cubos::engine::PhysicsBundle.
/// @ingroup physics-plugin

#pragma once

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/physics/components/physics_material.hpp>

namespace cubos::engine
{
    /// @brief Component which encapsulates the creation all components required for physics.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API PhysicsBundle
    {
        CUBOS_REFLECT;

        float mass = 1.0F;
        glm::vec3 velocity = {0.0F, 0.0F, 0.0F};
        glm::vec3 angularVelocity = {0.0F, 0.0F, 0.0F};
        glm::vec3 force = {0.0F, 0.0F, 0.0F};
        glm::vec3 torque = {0.0F, 0.0F, 0.0F};
        glm::vec3 impulse = {0.0F, 0.0F, 0.0F};
        glm::vec3 angularImpulse = {0.0F, 0.0F, 0.0F};
        PhysicsMaterial material = PhysicsMaterial{};
        glm::mat3 inertiaTensor = glm::mat3(0.0F); // Temporary value. We use this value to check if the inertia tensor
                                                   // is to be calculated automatically.
    };
} // namespace cubos::engine
