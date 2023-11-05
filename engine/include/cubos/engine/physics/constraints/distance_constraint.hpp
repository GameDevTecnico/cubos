/// @file
/// @brief Component @ref cubos::engine::PhysicsDistanceConstraint.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

/*
namespace cubos::engine
{
    /// @brief Component which defines parameters of a constraint.
    /// @ingroup physics-plugin
    struct [[cubos::component("cubos/physics_distance_constraint", VecStorage)]] PhysicsDistanceConstrain
    {
        Entity entity1;
        Entity entity2;
        float restLength;
        float lagrange;
        float compliance;
        glm::vec3 force;
    };

    static void DistanceConstrainSolverSystem(Query<PhysicsDistanceConstrain> constraints)
    {
        for (auto constrain : contraints)
        {
            auto ent1 = Query<Position, Velocity>(constrain->entity1);
            auto ent2 = Query<Position, Velocity>(constrain->entity2);

            auto [entity1, position1, velocity1] = ent1;
            auto [entity2, position2, velocity2] = ent2;

            glm::vec3 delta_x = position1->vec - position2->vec;

            float length = delta_x.length();

            float c = length - self.rest_length;

            // Avoid division by zero and unnecessary computation.
            if length < Scalar::EPSILON || c.abs() < Scalar::EPSILON {
                return Vector::ZERO;
            }

            // Normalized delta_x
            glm::vec3 n = delta_x / length;

            // Compute generalized inverse masses (method from PositionConstraint)
            float w1 = mass1->inverseMass;
            float w2 = mass2->inverseMass;
            let w = [w1, w2];

            // Constraint gradients, i.e. how the bodies should be moved
            // relative to each other in order to satisfy the constraint
            let gradients = [n, -n];

            // Compute Lagrange multiplier update, essentially the signed magnitude of the correction
            float delta_lagrange =
                self.compute_lagrange_update(self.lagrange, c, &gradients, &w, self.compliance, dt);
            self.lagrange += delta_lagrange;

            // Apply positional correction (method from PositionConstraint)
            self.apply_positional_correction(body1, body2, delta_lagrange, n, world_r1, world_r2);

            // Return constraint force
            self.compute_force(self.lagrange, n, dt);
        }
    }
} // namespace cubos::engine
*/
