#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../../fixed_substep/plugin.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::distanceConstraintWarmStartTag);
CUBOS_DEFINE_TAG(cubos::engine::distanceConstraintSolveTag);
CUBOS_DEFINE_TAG(cubos::engine::distanceConstraintCleanTag);

static void solveDistanceConstraint(
    Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&, AngularVelocity&,
          DistanceConstraint&, Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
          AngularVelocity&>
        query,
    const SolverConstants& solverConstants, const FixedDeltaTime& dt, const Substeps& substeps, const bool useBias)
{
    float subDeltaTime = dt.value / (float)substeps.value;
    for (auto [ent1, mass1, inertia1, rotation1, correction1, velocity1, angVelocity1, constraint, ent2, mass2,
               inertia2, rotation2, correction2, velocity2, angVelocity2] : query)
    {
        glm::vec3 r1 = rotation1.quat * constraint.localAnchor1;
        glm::vec3 r2 = rotation2.quat * constraint.localAnchor2;

        glm::vec3 ds = correction2.position - correction1.position + r2 - r1;
        glm::vec3 separation = constraint.deltaCenter + ds;

        float length = glm::length(separation);
        glm::vec3 axis = glm::normalize(separation);

        glm::vec3 vr =
            (velocity2.vec + glm::cross(angVelocity2.vec, r2)) - (velocity1.vec + glm::cross(angVelocity1.vec, r1));
        float cdot = glm::dot(vr, axis);

        if (constraint.isRigid)
        {
            float c = length - constraint.fixedDistance;

            /// TODO: check correctness of bias
            float bias = 0.0F;
            float massScale = 1.0F;
            float impulseScale = 0.0F;
            if (useBias)
            {
                bias = glm::max(constraint.biasCoefficient * c, solverConstants.maxBias);
                massScale = constraint.massCoefficient;
                impulseScale = constraint.impulseCoefficient;
            }

            float impulse = -massScale * constraint.axialMass * (cdot + bias) - impulseScale * constraint.impulse;
            constraint.impulse += impulse;

            glm::vec3 p = impulse * axis;
            velocity1.vec -= mass1.inverseMass * p;
            velocity2.vec += mass2.inverseMass * p;
            angVelocity1.vec -= inertia1.inverseInertia * glm::cross(r1, p);
            angVelocity2.vec += inertia2.inverseInertia * glm::cross(r2, p);
        }
        else
        {
            if (constraint.minDistance > 0.0F)
            {
                float c = length - constraint.minDistance;

                float bias = 0.0F;
                float massCoeff = 1.0F;
                float impulseCoeff = 0.0F;
                if (c > 0.0F)
                {
                    // speculative
                    bias = c * (1.0F / subDeltaTime);
                }
                else if (useBias)
                {
                    bias = glm::max(constraint.biasCoefficient * c, solverConstants.maxBias);
                    massCoeff = constraint.massCoefficient;
                    impulseCoeff = constraint.impulseCoefficient;
                }
                float impulse =
                    -massCoeff * constraint.axialMass * (cdot + bias) - impulseCoeff * constraint.lowerImpulse;
                float newImpulse = std::max(0.0F, constraint.lowerImpulse + impulse);
                impulse = newImpulse - constraint.lowerImpulse;
                constraint.lowerImpulse = newImpulse;

                glm::vec3 p = impulse * axis;
                velocity1.vec -= mass1.inverseMass * p;
                velocity2.vec += mass2.inverseMass * p;
                angVelocity1.vec -= inertia1.inverseInertia * glm::cross(r1, p);
                angVelocity2.vec += inertia2.inverseInertia * glm::cross(r2, p);
            }

            if (constraint.maxDistance > 0.0F)
            {
                float c = constraint.maxDistance - length;

                glm::vec3 vr = (velocity1.vec + glm::cross(angVelocity1.vec, r1)) -
                               (velocity2.vec + glm::cross(angVelocity2.vec, r2));
                float cdot = glm::dot(vr, axis);

                float bias = 0.0F;
                float massCoeff = 1.0F;
                float impulseCoeff = 0.0F;
                if (c > 0.0F)
                {
                    // speculative
                    bias = c * (1.0F / subDeltaTime);
                }
                else if (useBias)
                {
                    bias = glm::max(constraint.biasCoefficient * c, solverConstants.maxBias);
                    massCoeff = constraint.massCoefficient;
                    impulseCoeff = constraint.impulseCoefficient;
                }
                float impulse =
                    -massCoeff * constraint.axialMass * (cdot + bias) - impulseCoeff * constraint.upperImpulse;
                float newImpulse = std::max(0.0F, constraint.upperImpulse + impulse);
                impulse = newImpulse - constraint.upperImpulse;
                constraint.upperImpulse = newImpulse;

                glm::vec3 p = -impulse * axis;
                velocity1.vec -= mass1.inverseMass * p;
                velocity2.vec += mass2.inverseMass * p;
                angVelocity1.vec -= inertia1.inverseInertia * glm::cross(r1, p);
                angVelocity2.vec += inertia2.inverseInertia * glm::cross(r2, p);
            }
        }
    }
}

void cubos::engine::distanceConstraintPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.relation<DistanceConstraint>();

    cubos.tag(distanceConstraintWarmStartTag);
    cubos.tag(distanceConstraintSolveTag);
    cubos.tag(distanceConstraintCleanTag);

    cubos.system("solve distance constraints bias")
        .tagged(distanceConstraintSolveTag)
        .tagged(physicsSolveConstraintTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, DistanceConstraint&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query,
                 const SolverConstants& solverConstants, const FixedDeltaTime& dt,
                 const Substeps& substeps) { solveDistanceConstraint(query, solverConstants, dt, substeps, true); });

    cubos.system("solve distance constraints no bias")
        .tagged(physicsSolveRelaxConstraintTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, DistanceConstraint&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query,
                 const SolverConstants& solverConstants, const FixedDeltaTime& dt,
                 const Substeps& substeps) { solveDistanceConstraint(query, solverConstants, dt, substeps, false); });

    cubos.system("prepare constraints")
        .tagged(physicsPrepareSolveTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const CenterOfMass&, const LocalToWorld&, const Rotation&,
                       const Velocity&, const AngularVelocity&, const PhysicsMaterial&, DistanceConstraint&, Entity,
                       const Mass&, const Inertia&, const CenterOfMass&, const LocalToWorld&, const Rotation&,
                       const Velocity&, const AngularVelocity&, const PhysicsMaterial&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps,
                 const SolverConstants& solverConstants) {
            for (auto [ent1, mass1, inertia1, centerOfMass1, localToWorld1, rotation1, correction1, velocity1,
                       angVelocity1, constraint, ent2, mass2, inertia2, centerOfMass2, localToWorld2, rotation2,
                       correction2, velocity2, angVelocity2] : query)
            {

                float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;
                constraint.deltaCenter = localToWorld2.worldPosition() - localToWorld1.worldPosition();

                glm::vec3 r1 = rotation1.quat * constraint.localAnchor1;
                glm::vec3 r2 = rotation2.quat * constraint.localAnchor2;

                glm::vec3 separation = r2 - r1 + constraint.deltaCenter;
                glm::vec3 axis = glm::normalize(separation);

                glm::vec3 cross1 = glm::cross(r1, axis);
                glm::vec3 cross2 = glm::cross(r2, axis);

                float k = (mass1.inverseMass + mass2.inverseMass) + glm::dot(inertia1.inverseInertia * cross1, cross1) +
                          glm::dot(inertia2.inverseInertia * cross2, cross2);

                constraint.axialMass = (k > solverConstants.minKNormal) ? 1 / k : solverConstants.minKNormal;
                constraint.impulse = 0.0F;
                constraint.lowerImpulse = 0.0F;
                constraint.upperImpulse = 0.0F;

                // Soft contact
                const float zeta = 2.0F;
                float omega = 2.0F * glm::pi<float>() * 30.0F;
                float c = subDeltaTime * omega * (2.0F * zeta + subDeltaTime * omega);

                float biasCoefficient = omega / (2.0F * zeta + subDeltaTime * omega);
                float impulseCoefficient = 1.0F / (1.0F + c);
                float massCoefficient = c * impulseCoefficient;

                constraint.impulseCoefficient = impulseCoefficient;
                constraint.biasCoefficient = biasCoefficient;
                constraint.massCoefficient = massCoefficient;
            }
        });

    cubos.system("warm start distance constraints")
        .tagged(distanceConstraintWarmStartTag)
        .after(physicsPrepareSolveTag)
        .before(distanceConstraintSolveTag)
        .tagged(fixedSubstepTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, DistanceConstraint&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query) {
            for (auto [ent1, mass1, inertia1, rotation1, correction1, velocity1, angVelocity1, constraint, ent2, mass2,
                       inertia2, rotation2, correction2, velocity2, angVelocity2] : query)
            {
                glm::vec3 r1 = rotation1.quat * constraint.localAnchor1;
                glm::vec3 r2 = rotation2.quat * constraint.localAnchor2;

                glm::vec3 ds = correction2.position - correction1.position + r2 - r1;
                glm::vec3 separation = constraint.deltaCenter + ds;

                glm::vec3 axis = glm::normalize(separation);

                float axialImpulse = constraint.impulse + constraint.lowerImpulse - constraint.upperImpulse;

                glm::vec3 p = axialImpulse * axis;

                velocity1.vec -= mass1.inverseMass * p;
                velocity2.vec += mass2.inverseMass * p;
                angVelocity1.vec -= inertia1.inverseInertia * glm::cross(r1, p);
                angVelocity2.vec += inertia2.inverseInertia * glm::cross(r2, p);
            }
        });
}
