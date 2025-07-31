/// @file
/// @brief Component @ref cubos::engine::RigidBodyBundle.
/// @ingroup physics-plugin

#pragma once

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/physics/components/physics_material.hpp>

namespace cubos::engine
{
    /// @brief Component which encapsulates the creation all components required for a rigid body.
    /// A rigid body has all physical properties.
    ///
    /// Adds:
    /// - @ref ColliderBundle
    /// - @ref Velocity
    /// - @ref AngularVelocity
    /// - @ref Force
    /// - @ref Torque
    /// - @ref Impulse
    /// - @ref AngularImpulse
    /// - @ref Mass
    /// - @ref Inertia
    /// - @ref CenterOfMass
    /// - @ref AccumulatedCorrection
    /// - @ref PhysicsMaterial
    ///
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API RigidBodyBundle
    {
        CUBOS_REFLECT;

        /// @brief Whether the collider is active.
        bool isActive = true;

        /// @brief Layers of the collider.
        uint32_t layers = 0x00000001;

        /// @brief Mask of layers which the collider can collide with.
        uint32_t mask = 0x00000001;

        /// @brief Mass of the rigid body.
        float mass = 1.0F;

        /// @brief Initial velocity of the rigid body.
        glm::vec3 velocity = {0.0F, 0.0F, 0.0F};

        /// @brief Initial angular velocity of the rigid body.
        glm::vec3 angularVelocity = {0.0F, 0.0F, 0.0F};

        /// @brief Inital force applied on the rigid body.
        glm::vec3 force = {0.0F, 0.0F, 0.0F};

        /// @brief Initial torque applied on the rigid body.
        glm::vec3 torque = {0.0F, 0.0F, 0.0F};

        /// @brief Initial impulse applied on the rigid body.
        glm::vec3 impulse = {0.0F, 0.0F, 0.0F};

        /// @brief Initial angular impulse applied on the rigid body.
        glm::vec3 angularImpulse = {0.0F, 0.0F, 0.0F};

        /// @brief Physics material of the rigid body.
        PhysicsMaterial material = PhysicsMaterial{};

        /// @brief The center of mass of the rigid body.
        glm::vec3 centerOfMass = {0.0F, 0.0F, 0.0F};

        /// @brief Whether to perform automatic calculation of the inertia tensor according to the rigid body collision
        /// shape.
        bool autoInertiaTensor = true;

        /// @brief Inertia tensor for the rigid body. Not used if 'autoInertiaTensor' is set to true.
        glm::mat3 inertiaTensor = glm::mat3(0.0F);
    };
} // namespace cubos::engine
