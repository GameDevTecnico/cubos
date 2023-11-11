/// @file
/// @brief Gravity plugin, resource @ref cubos::engine::Gravity.
/// @ingroup physics-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

#include <cubos/engine/physics/resources/gravity.hpp>

#include <cubos/engine/physics/components/physics_velocity.hpp>
#include <cubos/engine/physics/components/physics_mass.hpp>

namespace cubos::engine
{
    /// @defgroup physics-plugin Physics
    /// @ingroup engine
    /// @brief Adds gravity to particles.
    /// ## Resources
    /// - @ref Gravity - holds the global value of gravity.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup physics-plugin
    void gravityPlugin(Cubos& cubos);
} // namespace cubos::engine
