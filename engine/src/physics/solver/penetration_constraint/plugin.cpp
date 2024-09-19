#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/collisions/contact_manifold.hpp>
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

void getTangents(const glm::vec3& velocity1, const glm::vec3& velocity2, const glm::vec3& n, glm::vec3& tangent1,
                 glm::vec3& tangent2)
{
    // Use linear relative velocity for determining tangents.
    /// TODO: test this with the normal way
    glm::vec3 linearVr = velocity2 - velocity1;

    tangent1 = linearVr - glm::dot(linearVr, n) * n;
    float tangentLenSq = glm::length2(tangent1);
    if (tangentLenSq > 1e-6 * 1e-6) /// TODO: check this
    {
        tangent1 = glm::normalize(tangent1);
        tangent2 = glm::cross(n, tangent1);
    }
    else
    {
        // if there is no tangent in relation that can be obtained from vr calculate a basis for the tangent
        // plane.
        getPlaneSpace(n, tangent1, tangent2);
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

void solvePenetrationConstraint(
    Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&, AngularVelocity&,
          PenetrationConstraint&, Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&,
          Velocity&, AngularVelocity&>
        query,
    const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps, const bool useBias)
{
    for (auto [ent1, mass1, inertia1, rotation1, correction1, velocity1, angVelocity1, constraint, ent2, mass2,
               inertia2, rotation2, correction2, velocity2, angVelocity2] : query)
    {

        float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

        glm::vec3 v1 = velocity1.vec;
        glm::vec3 v2 = velocity2.vec;

        glm::vec3 w1 = angVelocity1.vec;
        glm::vec3 w2 = angVelocity2.vec;

        // Separate bodies
        for (PenetrationConstraintPointData& contactPoint : constraint.points)
        {
            glm::vec3 r1 = rotation1.quat * contactPoint.localAnchor1;
            glm::vec3 r2 = rotation2.quat * contactPoint.localAnchor2;

            glm::vec3 deltaSeparation = correction2.position + r2 - correction1.position - r1;
            if (ent1 != constraint.entity)
            {
                deltaSeparation *= -1.0F;
            }

            float separation = glm::dot(deltaSeparation, constraint.normal) + contactPoint.initialSeparation;

            /// TODO: check correctness of bias
            float bias = 0.0F;
            float massScale = 1.0F;
            float impulseScale = 0.0F;
            if (separation > 0.0F)
            {
                bias = separation * (1.0F / subDeltaTime);
            }
            else if (useBias)
            {
                bias = glm::max(constraint.biasCoefficient * separation, -4.0F);
                massScale = constraint.massCoefficient;
                impulseScale = constraint.impulseCoefficient;
            }

            // Relative velocity at contact
            glm::vec3 vr1 = v1 + glm::cross(w1, r1);
            glm::vec3 vr2 = v2 + glm::cross(w2, r2);
            glm::vec3 vr = vr2 - vr1;
            if (ent1 != constraint.entity)
            {
                vr *= -1.0F;
            }

            float vn = glm::dot(vr, constraint.normal);

            // Compute normal impulse
            float impulse =
                -contactPoint.normalMass * massScale * (vn + bias) - impulseScale * contactPoint.normalImpulse;

            // Clamp the accumulated impulse
            float newImpulse = glm::max(contactPoint.normalImpulse + impulse, 0.0F);
            impulse = newImpulse - contactPoint.normalImpulse;
            contactPoint.normalImpulse = newImpulse;

            glm::vec3 p = impulse * constraint.normal;
            if (ent1 != constraint.entity)
            {
                p *= -1.0F;
            }

            v1 -= p * mass1.inverseMass;
            w1 -= inertia1.inverseInertia * glm::cross(r1, p);
            v2 += p * mass2.inverseMass;
            w2 += inertia2.inverseInertia * glm::cross(r2, p);
        }

        glm::vec3 tangent1;
        glm::vec3 tangent2;
        if (ent1 != constraint.entity)
        {
            getTangents(v2, v1, constraint.normal, tangent1, tangent2);
        }
        else
        {
            getTangents(v1, v2, constraint.normal, tangent1, tangent2);
        }

        // Friction
        for (PenetrationConstraintPointData& contactPoint : constraint.points)
        {
            glm::vec3 r1 = contactPoint.fixedAnchor1;
            glm::vec3 r2 = contactPoint.fixedAnchor2;

            // Relative velocity at contact
            glm::vec3 vr1 = v1 + glm::cross(w1, r1);
            glm::vec3 vr2 = v2 + glm::cross(w2, r2);
            glm::vec3 vr = vr2 - vr1;
            if (ent1 != constraint.entity)
            {
                vr *= -1.0F;
            }

            float vn1 = glm::dot(vr, tangent1);
            float vn2 = glm::dot(vr, tangent2);

            // Compute friction force
            float impulse1 = -contactPoint.frictionMass1 * vn1;
            float impulse2 = -contactPoint.frictionMass2 * vn2;

            // Clamp the accumulated force
            float maxFriction = constraint.friction * contactPoint.normalImpulse;
            float newImpulse1 = glm::clamp(contactPoint.frictionImpulse1 + impulse1, -maxFriction, maxFriction);
            float newImpulse2 = glm::clamp(contactPoint.frictionImpulse2 + impulse2, -maxFriction, maxFriction);
            impulse1 = newImpulse1 - contactPoint.frictionImpulse1;
            impulse2 = newImpulse2 - contactPoint.frictionImpulse2;
            contactPoint.frictionImpulse1 = newImpulse1;
            contactPoint.frictionImpulse2 = newImpulse2;

            // Apply contact impulse
            glm::vec3 p = impulse1 * tangent1 + impulse2 * tangent2;
            if (ent1 != constraint.entity)
            {
                p *= -1.0F;
            }

            v1 -= p * mass1.inverseMass;
            w1 -= inertia1.inverseInertia * glm::cross(r1, p);
            v2 += p * mass2.inverseMass;
            w2 += inertia2.inverseInertia * glm::cross(r2, p);
        }

        velocity1.vec = v1;
        angVelocity1.vec = w1;
        velocity2.vec = v2;
        angVelocity2.vec = w2;
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
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, PenetrationConstraint&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime,
                 const Substeps& substeps) { solvePenetrationConstraint(query, fixedDeltaTime, substeps, true); });

    cubos.system("solve contacts no bias")
        .tagged(penetrationConstraintSolveRelaxTag)
        .tagged(physicsSolveRelaxContactTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, PenetrationConstraint&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime,
                 const Substeps& substeps) { solvePenetrationConstraint(query, fixedDeltaTime, substeps, false); });

    cubos.system("add restitution")
        .tagged(penetrationConstraintRestitutionTag)
        .after(penetrationConstraintSolveRelaxTag)
        .before(physicsFinalizePositionTag)
        .tagged(fixedStepTag)
        .call([](Query<Entity, const Mass&, const Inertia&, AccumulatedCorrection&, Velocity&, AngularVelocity&,
                       PenetrationConstraint&, Entity, const Mass&, const Inertia&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&>
                     query) {
            for (auto [ent1, mass1, inertia1, correction1, velocity1, angVelocity1, constraint, ent2, mass2, inertia2,
                       correction2, velocity2, angVelocity2] : query)
            {
                if (constraint.restitution == 0.0F)
                {
                    continue;
                }

                glm::vec3 v1 = velocity1.vec;
                glm::vec3 v2 = velocity2.vec;
                glm::vec3 w1 = angVelocity1.vec;
                glm::vec3 w2 = angVelocity2.vec;

                for (auto point : constraint.points)
                {
                    if (point.normalSpeed > -0.01F || point.normalImpulse == 0.0F)
                    {
                        continue;
                    }

                    glm::vec3 r1 = point.fixedAnchor1;
                    glm::vec3 r2 = point.fixedAnchor2;

                    // Relative velocity at contact
                    glm::vec3 vr1 = v1 + glm::cross(w1, r1);
                    glm::vec3 vr2 = v2 + glm::cross(w2, r2);
                    glm::vec3 vr = vr2 - vr1;
                    if (ent1 != constraint.entity)
                    {
                        vr *= -1.0F;
                    }

                    float vn = glm::dot(vr, constraint.normal);

                    // compute normal impulse
                    float impulse = -point.normalMass * (vn + constraint.restitution * point.normalSpeed);

                    // Clamp the accumulated impulse
                    float newImpulse = glm::max(point.normalImpulse + impulse, 0.0F);
                    impulse = newImpulse - point.normalImpulse;
                    point.normalImpulse = newImpulse;

                    // Apply impulse
                    glm::vec3 p = constraint.normal * impulse;
                    v1 -= p * mass1.inverseMass;
                    w1 -= inertia1.inverseInertia * glm::cross(r1, p);
                    v2 += p * mass2.inverseMass;
                    w2 += inertia2.inverseInertia * glm::cross(r2, p);
                }

                velocity1.vec = v1;
                angVelocity1.vec = w1;
                velocity2.vec = v2;
                angVelocity2.vec = w2;
            }
        });

    cubos.system("add penetration constraint pair")
        .tagged(addPenetrationConstraintTag)
        .tagged(physicsPrepareSolveTag)
        .call([](Commands cmds,
                 Query<Entity, const Mass&, const Inertia&, const CenterOfMass&, const Rotation&, const Velocity&,
                       const AngularVelocity&, const PhysicsMaterial&, const ContactManifold&, Entity, const Mass&,
                       const Inertia&, const CenterOfMass&, const Rotation&, const Velocity&, const AngularVelocity&,
                       const PhysicsMaterial&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;
            float contactHertz = glm::min(30.0F, 0.25F * (1.0F / subDeltaTime));

            for (auto [ent1, mass1, inertia1, centerOfMass1, rotation1, velocity1, angVelocity1, material1, manifold,
                       ent2, mass2, inertia2, centerOfMass2, rotation2, velocity2, angVelocity2, material2] : query)
            {
                glm::vec3 tangent1;
                glm::vec3 tangent2;
                if (ent1 != manifold.entity)
                {
                    getTangents(velocity2.vec, velocity1.vec, manifold.normal, tangent1, tangent2);
                }
                else
                {
                    getTangents(velocity1.vec, velocity2.vec, manifold.normal, tangent1, tangent2);
                }

                std::vector<PenetrationConstraintPointData> points;
                for (auto point : manifold.points)
                {
                    auto pointData = PenetrationConstraintPointData{};

                    /// TODO: when we have warm-start change this
                    pointData.normalImpulse = 0.0F;
                    pointData.frictionImpulse1 = 0.0F;
                    pointData.frictionImpulse2 = 0.0F;

                    pointData.localAnchor1 = point.localOn1 - centerOfMass1.vec;
                    pointData.localAnchor2 = point.localOn2 - centerOfMass2.vec;

                    glm::vec3 r1 = rotation1.quat * pointData.localAnchor1;
                    glm::vec3 r2 = rotation2.quat * pointData.localAnchor2;

                    pointData.fixedAnchor1 = r1;
                    pointData.fixedAnchor2 = r2;

                    pointData.initialSeparation = -point.penetration - glm::dot(r2 - r1, manifold.normal);

                    // Relative velocity at contact
                    glm::vec3 vr1 = velocity1.vec + glm::cross(angVelocity1.vec, r1);
                    glm::vec3 vr2 = velocity2.vec + glm::cross(angVelocity2.vec, r2);
                    glm::vec3 vr = vr2 - vr1;
                    if (ent1 != manifold.entity)
                    {
                        vr *= -1.0F;
                    }

                    pointData.normalSpeed = glm::dot(vr, manifold.normal);

                    // normal mass
                    glm::vec3 rn1 = glm::cross(r1, manifold.normal);
                    glm::vec3 rn2 = glm::cross(r2, manifold.normal);
                    float kNormal = mass1.inverseMass + mass2.inverseMass +
                                    glm::dot(inertia1.inverseInertia * rn1, rn1) +
                                    glm::dot(inertia2.inverseInertia * rn2, rn2);
                    pointData.normalMass = kNormal > 0.0F ? 1.0F / kNormal : 0.0F;

                    // friction mass
                    glm::vec3 rt11 = glm::cross(r1, tangent1);
                    glm::vec3 rt12 = glm::cross(r2, tangent1);
                    glm::vec3 rt21 = glm::cross(r1, tangent2);
                    glm::vec3 rt22 = glm::cross(r2, tangent2);

                    // Multiply by the inverse inertia early to reuse the values
                    glm::vec3 i1Rt11 = inertia1.inverseInertia * rt11;
                    glm::vec3 i2Rt12 = inertia2.inverseInertia * rt12;
                    glm::vec3 i1Rt21 = inertia1.inverseInertia * rt21;
                    glm::vec3 i2Rt22 = inertia2.inverseInertia * rt22;

                    float kFriction1 =
                        mass1.inverseMass + mass2.inverseMass + glm::dot(i1Rt11, rt11) + glm::dot(i2Rt12, rt12);
                    float kFriction2 =
                        mass1.inverseMass + mass2.inverseMass + glm::dot(i1Rt21, rt21) + glm::dot(i2Rt22, rt22);

                    /// TODO: these could be an array in the point
                    pointData.frictionMass1 = kFriction1 > 0.0F ? 1.0F / kFriction1 : 0.0F;
                    pointData.frictionMass2 = kFriction2 > 0.0F ? 1.0F / kFriction2 : 0.0F;

                    points.push_back(pointData);
                }

                // determine friction
                float friction = mixValues(material1.friction, material2.friction,
                                           getMixProperty(material1.frictionMix, material2.frictionMix));

                // determine restitution
                float restitution = mixValues(material1.bounciness, material2.bounciness,
                                              getMixProperty(material1.bouncinessMix, material2.bouncinessMix));

                // Soft contact
                const float zeta = 10.0F;
                float omega = 2.0F * glm::pi<float>() * contactHertz;
                float c = subDeltaTime * omega * (2.0F * zeta + subDeltaTime * omega);

                float biasCoefficient = omega / (2.0F * zeta + subDeltaTime * omega);
                float impulseCoefficient = 1.0F / (1.0F + c);
                float massCoefficient = c * impulseCoefficient;

                cmds.relate(ent1, ent2,
                            PenetrationConstraint{.entity = manifold.entity,
                                                  .normal = manifold.normal,
                                                  .friction = friction,
                                                  .restitution = restitution,
                                                  .points = points,
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

// cleanup code
// change name in creation of manifold