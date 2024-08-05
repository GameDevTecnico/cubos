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
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintRestitutionTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintCleanTag);

void getPlaneSpace(const glm::vec3& n, glm::vec3& tangent1, glm::vec3& tangent2)
{
    if (glm::abs(n.z) > (1.0F / glm::sqrt(2.0F)))
    {
        tangent1 = glm::normalize(glm::vec3(0.0F, -n.z, n.y));
        tangent2 = glm::cross(n, tangent1);
    }
    else
    {
        tangent1 = glm::normalize(glm::vec3(-n.y, n.x, 0.0F));
        tangent2 = glm::cross(n, tangent1);
    }
}

static PhysicsMaterial::MixProperty getMixProperty(PhysicsMaterial::MixProperty mixProperty1,
                                                   PhysicsMaterial::MixProperty mixProperty2)
{
    return mixProperty1 > mixProperty2 ? mixProperty2 : mixProperty1;
}

static float mixValues(float value1, float value2, PhysicsMaterial::MixProperty mixProperty)
{
    switch (mixProperty)
    {
    case PhysicsMaterial::MixProperty::Maximum:
        return value1 > value2 ? value1 : value2;
    case PhysicsMaterial::MixProperty::Multiply:
        return value1 * value2;
    case PhysicsMaterial::MixProperty::Minimum:
        return value1 > value2 ? value2 : value1;
    case PhysicsMaterial::MixProperty::Average:
        return (value1 + value2) / 2.0F;
    default:
        return 0.0F;
    }
}

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

        // for each contact point in the future, since we have particles now we only need to do it once
        {
            float totalImpulse = constraint.normalImpulse;

            // compute current penetration
            float penetration = -constraint.penetration;

            glm::vec3 deltaPenetration = correction2.position - correction1.position;

            if (ent1 != constraint.entity)
            {
                deltaPenetration *= -1.0F;
            }

            penetration += glm::dot(deltaPenetration, constraint.normal);

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
            glm::vec3 vr = velocity2.vec - velocity1.vec;

            if (ent1 != constraint.entity)
            {
                vr *= -1.0F;
            }

            float vn = glm::dot(vr, constraint.normal);

            // Compute normal impulse
            float impulse = -constraint.normalMass * massScale * (vn + bias) - impulseScale * totalImpulse;

            // Clamp the accumulated impulse
            float newImpulse = glm::max(totalImpulse + impulse, 0.0F);
            impulse = newImpulse - totalImpulse;
            constraint.normalImpulse = newImpulse;

            glm::vec3 p = impulse * constraint.normal;

            if (ent1 != constraint.entity)
            {
                p *= -1.0F;
            }

            v1 = velocity1.vec - p * mass1.inverseMass;
            v2 = velocity2.vec + p * mass2.inverseMass;
        }

        // Friction
        // for each contact point in the future, since we have particles now we only need to do it once
        {
            float normalImpulse = constraint.normalImpulse;
            float frictionImpulse1 = constraint.frictionImpulse1;
            float frictionImpulse2 = constraint.frictionImpulse2;

            // Relative velocity at contact
            glm::vec3 vr = velocity2.vec - velocity1.vec;

            if (ent1 != constraint.entity)
            {
                vr *= -1.0F;
            }

            glm::vec3 tangent1 = vr - glm::dot(vr, constraint.normal) * constraint.normal;
            glm::vec3 tangent2;
            float tangentLenSq = glm::length2(tangent1);
            if (tangentLenSq > 1e-6 * 1e-6)
            {
                tangent1 = glm::normalize(tangent1);
                tangent2 = glm::cross(constraint.normal, tangent1);
            }
            else
            {
                // if there is no tangent in relation that can be obtained from vr calculate a basis for the tangent
                // plane.
                getPlaneSpace(constraint.normal, tangent1, tangent2);
            }

            float vn1 = glm::dot(vr, tangent1);
            float vn2 = glm::dot(vr, tangent2);

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
            glm::vec3 p = impulse1 * tangent1 + impulse2 * tangent2;

            if (ent1 != constraint.entity)
            {
                p *= -1.0F;
            }

            v1 -= p * mass1.inverseMass;
            v2 += p * mass2.inverseMass;
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
    cubos.tag(penetrationConstraintRestitutionTag);
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

    cubos.system("add restitution")
        .tagged(penetrationConstraintRestitutionTag)
        .after(penetrationConstraintSolveRelaxTag)
        .before(physicsFinalizePositionTag)
        .tagged(fixedStepTag)
        .call([](Query<Entity, const Mass&, AccumulatedCorrection&, Velocity&, PenetrationConstraint&, Entity,
                       const Mass&, AccumulatedCorrection&, Velocity&>
                     query) {
            for (auto [ent1, mass1, correction1, velocity1, constraint, ent2, mass2, correction2, velocity2] : query)
            {
                if (constraint.restitution == 0.0F)
                {
                    continue;
                }

                if (constraint.relativeVelocity > -0.01F || constraint.normalImpulse == 0.0F)
                {
                    continue;
                }

                // Relative normal velocity at contact
                glm::vec3 vr = velocity2.vec - velocity1.vec;

                if (ent1 != constraint.entity)
                {
                    vr *= -1.0F;
                }
                float vn = glm::dot(vr, constraint.normal);

                // compute normal impulse
                float impulse = -constraint.normalMass * (vn + constraint.restitution * constraint.relativeVelocity);

                // Clamp the accumulated impulse
                float newImpulse = glm::max(constraint.normalImpulse + impulse, 0.0F);
                impulse = newImpulse - constraint.normalImpulse;
                constraint.normalImpulse = newImpulse;

                // Apply impulse
                glm::vec3 p = constraint.normal * impulse;
                velocity1.vec = velocity1.vec - p * mass1.inverseMass;
                velocity2.vec = velocity2.vec + p * mass2.inverseMass;
            }
        });

    cubos.system("add penetration constraint pair")
        .tagged(addPenetrationConstraintTag)
        .tagged(physicsPrepareSolveTag)
        .call([](Commands cmds,
                 Query<Entity, const Mass&, const Velocity&, const PhysicsMaterial&, const CollidingWith&, Entity,
                       const Mass&, const Velocity&, const PhysicsMaterial&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;
            float contactHertz = glm::min(30.0F, 0.25F * (1.0F / subDeltaTime));

            for (auto [ent1, mass1, velocity1, material1, collidingWith, ent2, mass2, velocity2, material2] : query)
            {
                float kNormal = mass1.inverseMass + mass2.inverseMass;
                float normalMass = kNormal > 0.0F ? 1.0F / kNormal : 0.0F;

                // friction mass
                float kFriction = mass1.inverseMass + mass2.inverseMass;
                float frictionMass = kFriction > 0.0F ? 1.0F / kFriction : 0.0F;

                // determine friction
                float friction = mixValues(material1.friction, material2.friction,
                                           getMixProperty(material1.frictionMix, material2.frictionMix));

                // determine restitution
                float restitution = mixValues(material1.bounciness, material2.bounciness,
                                              getMixProperty(material1.bouncinessMix, material2.bouncinessMix));
                glm::vec3 vr = velocity2.vec - velocity1.vec;

                if (ent1 != collidingWith.entity)
                {
                    vr *= -1.0F;
                }
                float relativeVelocity = glm::dot(vr, collidingWith.normal);

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
                                                  .restitution = restitution,
                                                  .relativeVelocity = relativeVelocity,
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
