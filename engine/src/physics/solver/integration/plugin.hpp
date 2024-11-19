/// @file
/// @brief Integration plugin.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "physics_constants_integration.hpp"

namespace cubos::engine
{
    /// @ingroup physics-solver-plugin
    /// @brief Adds velocity and position integration.
    ///
    /// ## Resources
    /// - @ref PhysicsConstantsIntegration - holds values to resolve magic numbers.
    ///

    extern Tag physicsApplyImpulsesTag;
    extern Tag physicsClearForcesTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-solver-plugin
    void physicsIntegrationPlugin(Cubos& cubos);
} // namespace cubos::engine