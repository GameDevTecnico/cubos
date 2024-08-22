/// @file
/// @brief Component @ref cubos::engine::CenterOfMass.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines the center of mass of a body.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API CenterOfMass
    {
        CUBOS_REFLECT;

        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< Position of the center in local space.
    };
} // namespace cubos::engine
