/// @file
/// @brief Penetration Constraint plugin, resource @ref cubos::engine::PenetrationConstraintPlugin.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "../../constraints/penetration_constraint.hpp"

namespace cubos::engine
{
    /// @ingroup physics-solver-plugin
    /// @brief Adds solver for penetration constraint.

    extern Tag addPenetrationConstraintTag;
    extern Tag penetrationConstraintCleanTag;
    extern Tag penetrationConstraintSolveTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-solver-plugin
    void penetrationConstraintPlugin(Cubos& cubos);
} // namespace cubos::engine