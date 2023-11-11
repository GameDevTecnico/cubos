/// @file
/// @brief Gravity plugin, resource @ref cubos::engine::Gravity.
/// @ingroup physics-gravity-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/physics/components/physics_mass.hpp>
#include <cubos/engine/physics/components/physics_velocity.hpp>
#include <cubos/engine/physics/resources/gravity.hpp>

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
