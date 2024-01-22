/// @file
/// @brief Component @ref cubos::engine::Velocity.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which holds velocity and forces applied on a particle.
    /// @note Should be used with @ref Mass and @ref Position.
    /// @ingroup physics-plugin
    struct Velocity
    {
        CUBOS_REFLECT;

        glm::vec3 vec;
    };
} // namespace cubos::engine
