/// @file
/// @brief Penetration Constraint plugin.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "../../constraints/penetration_constraint.hpp"

namespace cubos::engine
{
    /// @ingroup physics-solver-plugin
    /// @brief Adds solver for penetration constraint.

    extern Tag addPenetrationConstraintTag;
    extern Tag penetrationConstraintWarmStartTag;
    extern Tag penetrationConstraintCleanTag;
    extern Tag penetrationConstraintSolveTag;
    extern Tag penetrationConstraintRestitutionTag;
    extern Tag penetrationConstraintSolveRelaxTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-solver-plugin
    void penetrationConstraintPlugin(Cubos& cubos);
} // namespace cubos::engine