/// @file
/// @brief Component @ref cubos::engine::SpringConstraint.
/// @ingroup physics-solver-plugin

#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Relation which holds the information for a spring constraint between two bodies.
    /// @ingroup physics-solver-plugin
    struct SpringConstraint
    {
        CUBOS_REFLECT;
        float stiffness;        ///< Spring stiffness (force per unit extension)
        float damping;          ///< Damping coefficient
        float restLength;       ///< Rest length of spring
        glm::vec3 localAnchor1; ///< The local contact point relative to the center of mass of the first body.
        glm::vec3 localAnchor2; ///< The local contact point relative to the center of mass of the second body.
    };
} // namespace cubos::engine
