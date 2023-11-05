/// @dir
/// @brief @ref physics-plugin plugin directory.

/// @file
/// @brief Plugin entry point, resources @ref cubos::engine::FixedDeltaTime and @ref cubos::engine::Accumulator and components @ref
/// cubos::engine::PhysicsVelocity and @ref cubos::engine::PhysicsMass.
/// @ingroup physics-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

#include <cubos/engine/physics/resources/accumulator.hpp>
#include <cubos/engine/physics/resources/damping.hpp>
#include <cubos/engine/physics/resources/fixed_delta_time.hpp>
#include <cubos/engine/physics/resources/gravity.hpp>
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
    /// ## Settings
    /// -
    ///
    /// ## Resources
    /// - @ref FixedDeltaTime - holds the value of the fixed delta for the physics update.
    /// - @ref Accumulator - holds the progress to the next integration.
    /// - @ref Damping - holds the damping value for integration.
    /// - @ref Gravity - holds the global value of gravity.
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
    /// ## Tags
    /// - `cubos.physics.integrate`

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-plugin
    void physicsPlugin(Cubos& cubos);
} // namespace cubos::engine
