/// @file
/// @brief Gravity plugin, resource @ref cubos::engine::Gravity.
/// @ingroup physics-gravity-plugin

#pragma once

#include <cubos/engine/physics/components/force.hpp>
#include <cubos/engine/physics/components/impulse.hpp>
#include <cubos/engine/physics/components/physics_mass.hpp>
#include <cubos/engine/physics/components/velocity.hpp>
#include <cubos/engine/physics/resources/gravity.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup physics-gravity-plugin Gravity
    /// @ingroup physics-plugin
    /// @brief Adds gravity to particles.
    ///
    /// ## Resources
    /// - @ref Gravity - holds the global value of gravity.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-gravity-plugin
    void gravityPlugin(Cubos& cubos);
} // namespace cubos::engine
