/// @dir
/// @brief @ref physics-plugin plugin directory.

/// @file
/// @brief Plugin entry point, resources @ref cubos::engine::FixedDeltaTime and @ref cubos::engine::Accumulator and components @ref
/// cubos::engine::PhysicsVelocity and @ref cubos::engine::PhysicsMass.
/// @ingroup physics-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

#include <cubos/engine/physics/plugins/gravity.hpp>

#include <cubos/engine/physics/resources/accumulator.hpp>
#include <cubos/engine/physics/resources/damping.hpp>
#include <cubos/engine/physics/resources/fixed_delta_time.hpp>
#include <cubos/engine/physics/resources/substeps.hpp>

#include <cubos/engine/physics/components/physics_velocity.hpp>
#include <cubos/engine/physics/components/physics_mass.hpp>
#include <cubos/engine/physics/components/previous_position.hpp>
#include <cubos/engine/physics/components/accumulated_correction.hpp>

namespace cubos::engine
{
    /// @defgroup physics-plugin Physics
    /// @ingroup engine
    /// @brief Creates and handles the physics simulation.
    /// @see Take a look at the @ref examples-engine-physics example for a demonstration of this
    /// plugin.
    ///
    /// ## Resources
    /// - @ref FixedDeltaTime - holds the value of the fixed delta for the physics update.
    /// - @ref Accumulator - holds the progress to the next integration.
    /// - @ref Damping - holds the damping value for integration.
    /// - @ref Substeps - holds the amount of substeps for the physics update.
    ///
    /// ## Components
    /// - @ref PhysicsVelocity - holds the information for moving an object straight.
    /// - @ref PhysicsMass - holds the mass of an object.
    /// - @ref PreviousPosition - holds the previous position of the entity in a substep.
    /// - @ref AccumulatedCorrection - holds the corrections accumulated from the constraints solving.
    ///
    /// ## Startup tags
    /// - do we need any?
    ///
    /// ## Internal Simulation Tags
    /// - `cubos.physics.simulation.prepare`
    /// - `cubos.physics.simulation.apply_impulses`
    /// - `cubos.physics.simulation.substeps.integrate`
    /// - `cubos.physics.simulation.substeps.correct_position`
    /// - `cubos.physics.simulation.substeps.update_velocity`
    /// - `cubos.physics.simulation.clear_forces`
    /// - `cubos.physics.simulation.decrease_accumulator`
    ///
    /// ## User Tags
    /// - `cubos.physics.apply_forces`

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-plugin
    void physicsPlugin(Cubos& cubos);
} // namespace cubos::engine
