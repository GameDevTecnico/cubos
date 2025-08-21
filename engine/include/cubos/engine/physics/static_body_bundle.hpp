/// @file
/// @brief Component @ref cubos::engine::StaticBodyBundle.
/// @ingroup physics-plugin

#pragma once

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/physics/components/physics_material.hpp>

namespace cubos::engine
{
    /// @brief Component which encapsulates the creation all physics components required for a static body.
    /// A static body has all physical properties but is physically immovable (its mass is infinite).
    ///
    /// May be moved by changing the entity Transform directly, however, this may create abnormal behaviour when
    /// interacting with Rigid Bodies.
    ///
    /// Check also @ref RigidBodyBundle
    ///
    /// Adds:
    /// - @ref ColliderBundle
    /// - @ref Velocity
    /// - @ref AngularVelocity
    /// - @ref Mass
    /// - @ref Inertia
    /// - @ref CenterOfMass
    /// - @ref AccumulatedCorrection
    /// - @ref PhysicsMaterial
    ///
    /// @note A CollisionShape must also be added to the entity.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API StaticBodyBundle
    {
        CUBOS_REFLECT;

        /// @brief Whether the collider is active.
        bool isActive = true;

        /// @brief Layers of the collider.
        uint32_t layers = 0x00000001;

        /// @brief Mask of layers which the collider can collide with.
        uint32_t mask = 0x00000001;

        /// @brief Physics material of the rigid body.
        PhysicsMaterial material = PhysicsMaterial{};

        /// @brief The center of mass of the rigid body.
        glm::vec3 centerOfMass = {0.0F, 0.0F, 0.0F};
    };
} // namespace cubos::engine
