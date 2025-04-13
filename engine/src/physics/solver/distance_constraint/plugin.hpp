/// @file
/// @brief Distance Constraint plugin.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/physics/constraints/distance_constraint.hpp>
#include <cubos/engine/prelude.hpp>


namespace cubos::engine
{
    /// @ingroup physics-solver-plugin
    /// @brief Adds solver for distance constraint.

    extern Tag distanceConstraintWarmStartTag;
    extern Tag distanceConstraintCleanTag;
    extern Tag distanceConstraintSolveTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-solver-plugin
    void distanceConstraintPlugin(Cubos& cubos);
} // namespace cubos::engine