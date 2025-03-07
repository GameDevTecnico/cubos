/// @file
/// @brief Component @ref cubos::engine::Mass.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines the mass of a particle.
    ///
    /// Directly setting density for a body is not supported. However, you can set mass based on density by considering
    /// `mass = volume * density`.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Mass
    {
        CUBOS_REFLECT;

        float mass;
        float inverseMass;

        bool changed = true;
    };
} // namespace cubos::engine
