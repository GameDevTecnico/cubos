/// @dir
/// @brief @ref fixed-step-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup fixed-step-plugin

#pragma once

#include <cubos/engine/fixed_substep/substeps.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup fixed-step-plugin Fixed Time Step
    /// @ingroup engine
    /// @brief Adds a tag which makes its systems run at a fixed frame rate.
    ///
    /// ## Resources
    /// - @ref FixedDeltaTime - holds the value of the fixed delta for the physics update.
    ///

    /// @brief Systems with this tag run at a fixed framerate.
    extern Tag fixedSubstepTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup fixed-step-plugin
    void fixedSubstepPlugin(Cubos& cubos);
} // namespace cubos::engine