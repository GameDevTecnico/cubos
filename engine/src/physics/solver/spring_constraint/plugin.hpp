/// @file
/// @brief Spring Constraint plugin.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/physics/constraints/spring_constraint.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @ingroup physics-solver-plugin
    /// @brief Adds solver for spring constraint.

    /// @brief Tag for the spring constraint solve system.
    extern Tag springConstraintSolveTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-solver-plugin
    void springConstraintPlugin(Cubos& cubos);
} // namespace cubos::engine
