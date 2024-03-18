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
#include <cubos/engine/physics/components/force.hpp>
#include <cubos/engine/physics/components/impulse.hpp>
#include <cubos/engine/physics/components/mass.hpp>
#include <cubos/engine/physics/components/previous_position.hpp>
#include <cubos/engine/physics/components/velocity.hpp>
#include <cubos/engine/physics/physics_bundle.hpp>
#include <cubos/engine/physics/resources/damping.hpp>
#include <cubos/engine/physics/resources/substeps.hpp>
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
    /// - @ref Force - holds forces applied on a particle.
    /// - @ref Impulse - holds impulses applied on a particle.
    /// - @ref Mass - holds the mass of an object.
    /// - @ref PreviousPosition - holds the previous position of the entity in a substep.
    /// - @ref AccumulatedCorrection - holds the corrections accumulated from the constraints solving.
    ///
    /// ## Dependencies
    /// - @ref physics-gravity-plugin

    extern Tag physicsApplyForcesTag;

    extern Tag physicsSimulationApplyImpulsesTag;

    extern Tag physicsSimulationSubstepsIntegrateTag;

    extern Tag physicsSimulationSubstepsCorrectPositionTag;

    extern Tag physicsSimulationSubstepsUpdateVelocityTag;

    extern Tag physicsSimulationClearForcesTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-plugin
    void physicsPlugin(Cubos& cubos);
} // namespace cubos::engine
