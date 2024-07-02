#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../../fixed_substep/plugin.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::addPenetrationConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintSolveTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintSolveRelaxTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintCleanTag);

void solvePenetrationConstraint(Query<Entity, const Mass&, AccumulatedCorrection&, Velocity&, PenetrationConstraint&,
                                      Entity, const Mass&, AccumulatedCorrection&, Velocity&>
                                    query,
                                const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps, const bool useBias)
{
    for (auto [ent1, mass1, correction1, velocity1, constraint, ent2, mass2, correction2, velocity2] : query)
    {

        float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

        glm::vec3 v1 = velocity1.vec;
        glm::vec3 v2 = velocity2.vec;

        // for each contact point, for now its for each entity
        for (int i = 0; i < 2; i++)
        {
            float totalImpulse = constraint.normalImpulse;

            // compute current penetration
            float penetration = -constraint.penetration;

            if (ent1 == constraint.entity)
            {
                glm::vec3 deltaPenetration = correction2.position - correction1.position;
                penetration += glm::dot(deltaPenetration, constraint.normal);
            }
            else
            {
                glm::vec3 deltaPenetration = correction1.position - correction2.position;
                penetration += glm::dot(deltaPenetration, constraint.normal);
            }

            float bias = 0.0F;
            float massScale = 1.0F;
            float impulseScale = 0.0F;
            if (penetration > 0.0F)
            {
                bias = 0.2F * penetration * (1.0F / subDeltaTime);
            }
            else if (useBias)
            {
                bias = glm::max(constraint.biasCoefficient * penetration, -4.0F);
                massScale = constraint.massCoefficient;
                impulseScale = constraint.impulseCoefficient;
            }

            // Relative velocity at contact
            glm::vec3 vr2;
            glm::vec3 vr1;
            if (ent1 == constraint.entity)
            {
                vr2 = velocity2.vec;
                vr1 = velocity1.vec;
            }
            else
            {
                vr2 = velocity1.vec;
                vr1 = velocity2.vec;
            }

            float vn = glm::dot(vr2 - vr1, constraint.normal);

            // Compute normal impulse
            float impulse = -constraint.normalMass * massScale * (vn + bias) - impulseScale * totalImpulse;

            // Clamp the accumulated impulse
            float newImpulse = glm::max(totalImpulse + impulse, 0.0F);
            impulse = newImpulse - totalImpulse;
            if (i == 0)
            {
                constraint.normalImpulse = newImpulse;
            }
            else
            {
                constraint.normalImpulse = newImpulse;
            }

            glm::vec3 p = constraint.normal * impulse;
            if (ent1 == constraint.entity)
            {
                v1 = velocity1.vec - p * mass1.inverseMass;
                v2 = velocity2.vec + p * mass2.inverseMass;
            }
            else
            {
                v1 = velocity1.vec + p * mass1.inverseMass;
                v2 = velocity2.vec - p * mass2.inverseMass;
            }
        }

        // Friction
        float normalImpulse = constraint.normalImpulse;
        float frictionImpulse1 = constraint.frictionImpulse1;
        float frictionImpulse2 = constraint.frictionImpulse2;

        // Relative velocity at contact
        glm::vec3 vr2;
        glm::vec3 vr1;
        if (ent1 == constraint.entity)
        {
            vr2 = velocity2.vec;
            vr1 = velocity1.vec;
        }
        else
        {
            vr2 = velocity1.vec;
            vr1 = velocity2.vec;
        }

        glm::vec3 vr = vr2 - vr1;
        glm::vec3 tangent1 = vr - glm::dot(vr, constraint.normal) * constraint.normal;
        glm::vec3 tangent2;
        float tangentLenSq = glm::length2(tangent1);
        if (tangentLenSq > 1e-3)
        {
            tangent1 = glm::normalize(tangent1);
            tangent2 = glm::cross(constraint.normal, tangent1);

            float vn1 = glm::dot(vr, tangent1);
            float vn2 = glm::dot(vr, tangent2);
            if (vn1 < 1e-3)
            {
                vn1 = 0.0F;
            }
            if (vn2 < 1e-3)
            {
                vn2 = 0.0F;
            }

            // Compute friction force
            float impulse1 = -constraint.frictionMass * vn1;
            float impulse2 = -constraint.frictionMass * vn2;

            // Clamp the accumulated force
            float maxFriction = constraint.friction * normalImpulse;
            float newImpulse1 = glm::clamp(frictionImpulse1 + impulse1, -maxFriction, maxFriction);
            float newImpulse2 = glm::clamp(frictionImpulse2 + impulse2, -maxFriction, maxFriction);
            impulse1 = newImpulse1 - frictionImpulse1;
            impulse2 = newImpulse2 - frictionImpulse2;
            constraint.frictionImpulse1 = newImpulse1;
            constraint.frictionImpulse2 = newImpulse2;

            // Apply contact impulse
            glm::vec3 p1 = tangent1 * impulse1;
            glm::vec3 p2 = tangent2 * impulse2;
            if (ent1 == constraint.entity)
            {
                v1 -= p1 * mass1.inverseMass + p2 * mass1.inverseMass;
                v2 += p1 * mass2.inverseMass + p2 * mass2.inverseMass;
            }
            else
            {
                v1 += p1 * mass1.inverseMass + p2 * mass1.inverseMass;
                v2 -= p1 * mass2.inverseMass + p2 * mass2.inverseMass;
            }
        }

        velocity1.vec = v1;
        velocity2.vec = v2;
    }
}

void cubos::engine::penetrationConstraintPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.relation<PenetrationConstraint>();

    cubos.tag(addPenetrationConstraintTag);
    cubos.tag(penetrationConstraintSolveTag);
    cubos.tag(penetrationConstraintSolveRelaxTag);
    cubos.tag(penetrationConstraintCleanTag);

    cubos.system("solve contacts bias")
        .tagged(penetrationConstraintSolveTag)
        .tagged(physicsSolveContactTag)
        .call([](Query<Entity, const Mass&, AccumulatedCorrection&, Velocity&, PenetrationConstraint&, Entity,
                       const Mass&, AccumulatedCorrection&, Velocity&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime,
                 const Substeps& substeps) { solvePenetrationConstraint(query, fixedDeltaTime, substeps, true); });

    cubos.system("solve contacts no bias")
        .tagged(penetrationConstraintSolveRelaxTag)
        .tagged(physicsSolveRelaxContactTag)
        .call([](Query<Entity, const Mass&, AccumulatedCorrection&, Velocity&, PenetrationConstraint&, Entity,
                       const Mass&, AccumulatedCorrection&, Velocity&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime,
                 const Substeps& substeps) { solvePenetrationConstraint(query, fixedDeltaTime, substeps, false); });

    cubos.system("add penetration constraint pair")
        .tagged(addPenetrationConstraintTag)
        .tagged(physicsPrepareSolveTag)
        .call([](Commands cmds, Query<Entity, const Mass&, const CollidingWith&, Entity, const Mass&> query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;
            float contactHertz = glm::min(30.0F, 0.25F * (1.0F / subDeltaTime));

            for (auto [ent1, mass1, collidingWith, ent2, mass2] : query)
            {
                float kNormal = mass1.inverseMass + mass2.inverseMass;
                float normalMass = kNormal > 0.0F ? 1.0F / kNormal : 0.0F;

                // friction mass
                float kFriction = mass1.inverseMass + mass2.inverseMass;
                float frictionMass = kFriction > 0.0F ? 1.0F / kFriction : 0.0F;

                // determine friction (set to predefined value for now)
                float friction = 0.01F;

                // Soft contact
                const float zeta = 10.0F;
                float omega = 2.0F * glm::pi<float>() * contactHertz;
                float c = subDeltaTime * omega * (2.0F * zeta + subDeltaTime * omega);

                float biasCoefficient = omega / (2.0F * zeta + subDeltaTime * omega);
                float impulseCoefficient = 1.0F / (1.0F + c);
                float massCoefficient = c * impulseCoefficient;

                cmds.relate(ent1, ent2,
                            PenetrationConstraint{.entity = ent1,
                                                  .entity1OriginalPosition = collidingWith.entity1OriginalPosition,
                                                  .entity2OriginalPosition = collidingWith.entity2OriginalPosition,
                                                  .penetration = collidingWith.penetration,
                                                  .normal = collidingWith.normal,
                                                  .normalMass = normalMass,
                                                  .normalImpulse = 0.0F,
                                                  .friction = friction,
                                                  .frictionMass = frictionMass,
                                                  .frictionImpulse1 = 0.0F,
                                                  .frictionImpulse2 = 0.0F,
                                                  .biasCoefficient = biasCoefficient,
                                                  .impulseCoefficient = impulseCoefficient,
                                                  .massCoefficient = massCoefficient});
            }
        });

    cubos.system("clean penetration constraint pairs")
        .tagged(penetrationConstraintCleanTag)
        .before(addPenetrationConstraintTag)
        .tagged(fixedStepTag)
        .call([](Commands cmds, Query<Entity, PenetrationConstraint&, Entity> query) {
            for (auto [entity, constraint, other] : query)
            {
                cmds.unrelate<PenetrationConstraint>(entity, other);
            }
        });
}
