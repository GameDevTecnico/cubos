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
    ///
    /// Dynamic bodies rotate around this point.
    ///
    /// If set automatically, always corresponds to the center of the collision shape.
    /// Generally corresponds to the origin of the local space, since all shapes are build around the origin.
    /// The exception is when the collision shape has an offset.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API CenterOfMass
    {
        CUBOS_REFLECT;

        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< Position of the center in local space.
    };
} // namespace cubos::engine
