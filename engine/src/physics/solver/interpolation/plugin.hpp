/// @file
/// @brief Interpolation plugin.
/// @ingroup physics-solver-plugin

#pragma once

#include <cubos/engine/physics/components/interpolated.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @ingroup physics-solver-plugin
    /// @brief Interpolates position, rotation and scale of entities with the @ref Interpolated component in between
    /// fixed updates.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup physics-solver-plugin
    void interpolationPlugin(Cubos& cubos);
} // namespace cubos::engine