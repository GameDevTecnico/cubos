/// @dir
/// @brief @ref fixed-step-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup fixed-step-plugin

#pragma once

#include <cubos/engine/fixed_step/fixed_accumulated_time.hpp>
#include <cubos/engine/fixed_step/fixed_delta_time.hpp>
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
    CUBOS_ENGINE_API extern Tag fixedStepTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup fixed-step-plugin
    CUBOS_ENGINE_API void fixedStepPlugin(Cubos& cubos);
} // namespace cubos::engine