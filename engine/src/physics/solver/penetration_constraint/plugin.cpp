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

        glm::vec3 v1;
        glm::vec3 v2;

        // for each contact point, for now its for each entity
        for (int i = 0; i < 2; i++)
        {
            float totalImpulse = i == 0 ? correction1.impulse : correction2.impulse;

            // compute current penatration
            float penetration = constraint.penetration;

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

            // penetration is positive but we need negative value for separation
            penetration = -penetration;

            float bias = 0.0F;
            float massScale = 1.0F;
            float impulseScale = 0.0F;
            if (penetration > 0.0F)
            {
                bias = penetration * 1.0F / subDeltaTime;
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
                correction1.impulse = newImpulse;
            }
            else
            {
                correction2.impulse = newImpulse;
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
    cubos.depends(solverPlugin);

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
