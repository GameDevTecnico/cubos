/// @file
/// @brief Component @ref cubos::engine::PhysicsMass.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which defines the mass of a particle.
    /// @ingroup physics-plugin
    struct [[cubos::component("cubos/physics_mass", VecStorage)]] PhysicsMass
    {
        CUBOS_REFLECT;

        float mass;
        float inverseMass;
    };
} // namespace cubos::engine
