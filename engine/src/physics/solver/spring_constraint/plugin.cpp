#include "plugin.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../../fixed_substep/plugin.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::springConstraintSolveTag);

static void solveSpringConstraint(Query<Entity, const Mass&, const Inertia&, const Rotation&, const Position&,
                                        Velocity&, AngularVelocity&, SpringConstraint&, Entity, const Mass&,
                                        const Inertia&, const Rotation&, const Position&, Velocity&, AngularVelocity&>
                                      query,
                                  const FixedDeltaTime& dt, const Substeps& substeps)
{
    float subDeltaTime = dt.value / static_cast<float>(substeps.value);

    for (auto [ent1, mass1, inertia1, rotation1, position1, velocity1, angVelocity1, constraint, ent2, mass2, inertia2,
               rotation2, position2, velocity2, angVelocity2] : query)
    {
        glm::vec3 r1 = rotation1.quat * constraint.localAnchor1;
        glm::vec3 r2 = rotation2.quat * constraint.localAnchor2;

        glm::vec3 worldAnchor1 = position1.vec + r1;
        glm::vec3 worldAnchor2 = position2.vec + r2;

        glm::vec3 springVec = worldAnchor2 - worldAnchor1;
        float currentLength = glm::length(springVec);

        if (currentLength < 1e-6F)
        {
            continue;
        }

        glm::vec3 direction = springVec / currentLength;

        float extension = currentLength - constraint.restLength;

        float springForce = constraint.stiffness * extension;

        glm::vec3 vel1 = velocity1.vec + glm::cross(angVelocity1.vec, r1);
        glm::vec3 vel2 = velocity2.vec + glm::cross(angVelocity2.vec, r2);
        glm::vec3 relVel = vel2 - vel1;

        float dampingForce = constraint.damping * glm::dot(relVel, direction);

        float totalForce = springForce + dampingForce;

        float impulse = totalForce * subDeltaTime;
        glm::vec3 impulseVec = impulse * direction;

        velocity1.vec += impulseVec * mass1.inverseMass;
        velocity2.vec -= impulseVec * mass2.inverseMass;

        angVelocity1.vec += inertia1.inverseInertia * glm::cross(r1, impulseVec);
        angVelocity2.vec -= inertia2.inverseInertia * glm::cross(r2, impulseVec);
    }
}

void cubos::engine::springConstraintPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.relation<SpringConstraint>();

    cubos.tag(springConstraintSolveTag);

    cubos.system("solve spring constraints")
        .tagged(springConstraintSolveTag)
        .tagged(physicsSolveConstraintTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, const Position&, Velocity&,
                       AngularVelocity&, SpringConstraint&, Entity, const Mass&, const Inertia&, const Rotation&,
                       const Position&, Velocity&, AngularVelocity&>
                     query,
                 const FixedDeltaTime& dt, const Substeps& substeps) { solveSpringConstraint(query, dt, substeps); });
}
