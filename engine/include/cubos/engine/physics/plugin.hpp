/// @dir
/// @brief @ref physics-plugin plugin directory.

/// @file
/// @brief Plugin entry point, resources @ref cubos::engine::FixedDeltaTime and @ref cubos::engine::Accumulator and components @ref
/// cubos::engine::PhysicsVelocity and @ref cubos::engine::PhysicsMass.
/// @ingroup physics-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

#include <cubos/engine/physics/physics_velocity.hpp>
#include <cubos/engine/physics/physics_mass.hpp>

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
    /// - @ref FixedDeltaTime - holds the value of the fixed delta for the integrator.
    /// - @ref Accumulator - holds the progress to the next integration.
    ///
    /// ## Components
    /// - @ref PhysicsVelocity - holds the information for moving an object straight.
    /// - @ref PhysicsMass - hold the mass of an object.
    ///
    /// ## Startup tags
    /// - do we need any?
    ///
    /// ## Tags
    /// - `cubos.physics.integrate`

    struct FixedDeltaTime
    {
        float value = 1.0f/100.0F;
    };

    struct Accumulator
    {
        float value = 0.0F;
    };

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-plugin
    void physicsPlugin(Cubos& cubos);
} // namespace cubos::engine