/// @file
/// @brief Component @ref cubos::engine::DistanceConstraint.
/// @ingroup physics-solver-plugin

#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Relation which holds the information for resolving a limited distance between particles.
    /// @ingroup physics-solver-plugin
    struct DistanceConstraint
    {
        CUBOS_REFLECT;

        bool isRigid;          ///< Wether the constraint enforces a fixed distance or a range
        float fixedDistance;   ///< Fixed distance between anchor points
        float minDistance;     ///< Minimum distance between anchor points
        float maxDistance;     ///< Maximum distance between anchor points
        glm::vec3 deltaCenter; ///< The world-space vector from the center of the first body to the second.

        glm::vec3 localAnchor1; ///< The local contact point relative to the center of mass of the first body.
        glm::vec3 localAnchor2; ///< The local contact point relative to the center of mass of the second body.

        float axialMass; ///< Effective mass to use for impulse calculation

        float impulse;      ///< Stored impulse for warmstarting
        float lowerImpulse; ///< Stored impulse for warmstarting
        float upperImpulse; ///< Stored impulse for warmstarting

        // soft constraint
        float biasCoefficient;
        float impulseCoefficient;
        float massCoefficient;
    };

} // namespace cubos::engine