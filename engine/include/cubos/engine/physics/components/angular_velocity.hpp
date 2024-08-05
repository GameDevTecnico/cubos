/// @file
/// @brief Component @ref cubos::engine::AngularVelocity.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds angular velocity of a body.
    /// @note Should be used with @ref Inertia and @ref Rotation.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API AngularVelocity
    {
        CUBOS_REFLECT;

        glm::vec3 vec;
    };
} // namespace cubos::engine
