/// @file
/// @brief Fixed Substep plugin.
/// @ingroup physics-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "substeps.hpp"

namespace cubos::engine
{
    /// @ingroup physics-plugin
    /// @brief Adds a tag which makes its systems run at a fixed number of times within fixed delta update.
    ///
    /// ## Resources
    /// - @ref Substeps - holds the number of substeps for the physics update.
    ///

    /// @brief Systems with this tag run a set number of times within fixed physics update.
    extern Tag fixedSubstepTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-plugin
    void fixedSubstepPlugin(Cubos& cubos);
} // namespace cubos::engine