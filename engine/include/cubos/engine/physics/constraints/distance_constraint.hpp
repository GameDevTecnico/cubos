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
    /// @brief Relation that holds the information for a distance constraint between two physics bodies.
    /// @ingroup physics-solver-plugin
    struct DistanceConstraint
    {
        CUBOS_REFLECT;

        bool isRigid;               ///< Whether the constraint enforces a fixed distance or a range
        float fixedDistance = 0.0F; ///< Fixed distance between anchor points (only used in the rigid case)
        float minDistance = 0.0F;   ///< Minimum distance between anchor points (only used when non-rigid)
        float maxDistance = 0.0F;   ///< Maximum distance between anchor points (only used when non-rigid)
        glm::vec3 localAnchor1;     ///< The local contact point relative to the center of mass of the first body.
        glm::vec3 localAnchor2;     ///< The local contact point relative to the center of mass of the second body.

        glm::vec3 deltaCenter = {0.0F, 0.0F,
                                 0.0F}; ///< The world-space vector from the center of the first body to the second.

        float axialMass = 0.0F; ///< Effective mass to use for impulse calculation

        float impulse = 0.0F;      ///< Stored impulse for warmstarting
        float lowerImpulse = 0.0F; ///< Stored impulse for warmstarting
        float upperImpulse = 0.0F; ///< Stored impulse for warmstarting

        // soft constraint
        float biasCoefficient = 0.0F;
        float impulseCoefficient = 0.0F;
        float massCoefficient = 0.0F;
    };

} // namespace cubos::engine
