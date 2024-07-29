/// @dir
/// @brief @ref physics-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup physics-plugin

#pragma once

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/physics/components/accumulated_correction.hpp>
#include <cubos/engine/physics/components/angular_impulse.hpp>
#include <cubos/engine/physics/components/angular_velocity.hpp>
#include <cubos/engine/physics/components/center_of_mass.hpp>
#include <cubos/engine/physics/components/force.hpp>
#include <cubos/engine/physics/components/impulse.hpp>
#include <cubos/engine/physics/components/inertia.hpp>
#include <cubos/engine/physics/components/mass.hpp>
#include <cubos/engine/physics/components/physics_material.hpp>
#include <cubos/engine/physics/components/torque.hpp>
#include <cubos/engine/physics/components/velocity.hpp>
#include <cubos/engine/physics/physics_bundle.hpp>
#include <cubos/engine/physics/resources/damping.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup physics-plugin Physics
    /// @ingroup engine
    /// @brief Creates and handles the physics simulation.
    /// @see Take a look at the @ref examples-engine-physics example for a demonstration of this
    /// plugin.
    ///
    /// ## Resources
    /// - @ref Damping - holds the damping value for integration.
    /// - @ref Substeps - holds the amount of substeps for the physics update.
    ///
    /// ## Components
    /// - @ref PhysicsBundle - bundle that holds the physics information to give to a new entity.
    /// - @ref Velocity - holds the information for moving an object straight.
    /// - @ref AngularVelocity - holds the angular velocity of an object.
    /// - @ref Force - holds forces applied on a particle.
    /// - @ref Torque - holds torque applied on an object.
    /// - @ref Impulse - holds impulses applied on a particle.
    /// - @ref AngularImpulse - holds angular impulses applied on a particle.
    /// - @ref Mass - holds the mass of an object.
    /// - @ref Inertia - holds the inertia tensor of an object.
    /// - @ref CenterOfMass - holds the center of mass of an object.
    /// - @ref AccumulatedCorrection - holds the corrections accumulated from the constraints solving.
    /// - @ref PhysicsMaterial - holds the friction and bounciness properties of the particle.
    ///
    /// ## Dependencies
    /// - @ref physics-gravity-plugin

    /// @brief Tag which should be used on all systems that modify velocity or apply forces or impulses.
    CUBOS_ENGINE_API extern Tag physicsApplyForcesTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-plugin
    CUBOS_ENGINE_API void physicsPlugin(Cubos& cubos);
} // namespace cubos::engine
