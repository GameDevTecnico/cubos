/// @file
/// @brief Solver plugin, resource @ref cubos::engine::Solver.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/physics/components/force.hpp>
#include <cubos/engine/physics/components/impulse.hpp>
#include <cubos/engine/physics/components/mass.hpp>
#include <cubos/engine/physics/components/velocity.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup physics-solver-plugin Solver
    /// @ingroup physics-plugin
    /// @brief Adds solver for constraints.

    extern Tag PhysicsSolveTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-gravity-plugin
    void solverPlugin(Cubos& cubos);
} // namespace cubos::engine