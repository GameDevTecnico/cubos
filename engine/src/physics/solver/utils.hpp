#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// @brief Computes the lagrange multiplier update for a constraint.
    /// @param c Constraint error, zero if the constraint is satisfied.
    /// @param inverseMasses Inverse masses for all particles participating in the constraint.
    /// @param gradients Gradients for all particles participating in the constraint.
    /// @param numParticles Number of particles participating in the constraint.
    /// @param compliance Constraint's compliance.
    /// @param dt Substep delta time value.
    float getLagrangeMultiplierUpdate(float c, const float inverseMasses[], glm::vec3 gradients[], int numParticles,
                                      float compliance, float dt);
} // namespace cubos::engine