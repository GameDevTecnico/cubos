/// @file
/// @brief Resource @ref cubos::engine::SolverConstants.
/// @ingroup physics-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which allows configuration over constants in the solver plugin.
    /// @ingroup physics-plugin
    struct SolverConstants
    {
        CUBOS_REFLECT;

        /// @brief Minimum inverse mass for a body to be considered as imovable (as if it had infinite mass).
        float minInvMass = 0.0F;

        /// @brief Minimum inverse inertia for a body to rotate.
        glm::mat3 minInvInertia = glm::mat3(0.0F);

        /// @brief The maximum "boost" for the initial solving of the constraint. 
        float maxBias = -4.0F;

        /// @brief The minimum update frequency of the collision solving.
        float minContactHertz = 30.0F;

        /// @brief The minimum value for KNormal (to avoid divisions by zero).
        float minKNormal = 0.0F;

        /// @brief The minimum value for KFriction (to avoid divisions by zero).
        float minKFriction = 0.0F;

        /// @brief The minimum length of the tangent vector in the direction of the relative velocity (if it's below this we recalculate a basis for the tangents).
        float minTangentLenSq = 1e-6F * 1e-6F;

        /// @brief The minimum restitution value to calculate restitution for a body (restitution is bounciness).
        float minRestitution = 0.0F;

        /// @brief The minimum speed in the normal direction to calculate restitution for a body (lower is closer to elastic collisions).
        float minNormalSpeed = -0.01F;

        /// @brief The minimum impulse being applied in the normal direction to calculate restitution for a body (to avoid applying restitution when body position didn't change).
        float minNormalImpulse = 0.0F;
    };

} // namespace cubos::engine
