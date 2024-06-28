/// @file
/// @brief Solver plugin, resource @ref cubos::engine::Solver.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup physics-solver-plugin Solver
    /// @ingroup physics-plugin
    /// @brief Adds solver for constraints.

    extern Tag physicsPrepareSolveTag;
    extern Tag physicsIntegrateVelocityTag;
    extern Tag physicsSolveConstraintTag;
    extern Tag physicsSolveUserConstraintTag;
    extern Tag physicsSolveContactTag;
    extern Tag physicsIntegratePositionTag;
    extern Tag physicsSolveRelaxConstraintTag;
    extern Tag physicsSolveRelaxUserConstraintTag;
    extern Tag physicsSolveRelaxContactTag;
    extern Tag physicsFinalizePositionTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-gravity-plugin
    CUBOS_ENGINE_API void solverPlugin(Cubos& cubos);
} // namespace cubos::engine