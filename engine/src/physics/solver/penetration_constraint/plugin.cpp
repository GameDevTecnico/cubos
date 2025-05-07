#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../../fixed_substep/plugin.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::addPenetrationConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintWarmStartTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintSolveTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintSolveRelaxTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintRestitutionTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintCleanTag);

static void getPlaneSpace(const glm::vec3& n, glm::vec3& tangent1, glm::vec3& tangent2)
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

static void getTangents(const glm::vec3& velocity1, const glm::vec3& velocity2, const glm::vec3& n, glm::vec3& tangent1,
                        glm::vec3& tangent2, const SolverConstants& solverConstants)
{
    // Use linear relative velocity for determining tangents.
    glm::vec3 linearVr = velocity2 - velocity1;

    tangent1 = linearVr - glm::dot(linearVr, n) * n;
    float tangentLenSq = glm::length2(tangent1);
    if (tangentLenSq > solverConstants.minTangentLenSq) /// TODO: check this
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

static void solvePenetrationConstraint(
    Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&, AngularVelocity&,
          PenetrationConstraints&, Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&,
          Velocity&, AngularVelocity&>
        query,
    const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps, const SolverConstants& solverConstants,
    const bool useBias)
{
    float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

    for (auto [ent1, mass1, inertia1, rotation1, correction1, velocity1, angVelocity1, constraints, ent2, mass2,
               inertia2, rotation2, correction2, velocity2, angVelocity2] : query)
    {
        const Mass* matchedMass1 = &mass1;
        const Mass* matchedMass2 = &mass2;
        const Inertia* matchedInertia1 = &inertia1;
        const Inertia* matchedInertia2 = &inertia2;
        const Rotation* matchedRotation1 = &rotation1;
        const Rotation* matchedRotation2 = &rotation2;
        const AccumulatedCorrection* matchedCorrection1 = &correction1;
        const AccumulatedCorrection* matchedCorrection2 = &correction2;
        Velocity* matchedVelocity1 = &velocity1;
        Velocity* matchedVelocity2 = &velocity2;
        AngularVelocity* matchedAngVelocity1 = &angVelocity1;
        AngularVelocity* matchedAngVelocity2 = &angVelocity2;

        if (ent1 != constraints.entity)
        {
            std::swap(matchedMass1, matchedMass2);
            std::swap(matchedInertia1, matchedInertia2);
            std::swap(matchedRotation1, matchedRotation2);
            std::swap(matchedCorrection1, matchedCorrection2);
            std::swap(matchedVelocity1, matchedVelocity2);
            std::swap(matchedAngVelocity1, matchedAngVelocity2);
        }

        glm::vec3 v1 = matchedVelocity1->vec;
        glm::vec3 v2 = matchedVelocity2->vec;

        glm::vec3 w1 = matchedAngVelocity1->vec;
        glm::vec3 w2 = matchedAngVelocity2->vec;

        for (auto& constraint : constraints.penConstraints)
        {
            // Separate bodies
            for (PenetrationConstraintPointData& contactPoint : constraint.points)
            {
                glm::vec3 r1 = matchedRotation1->quat * contactPoint.localAnchor1;
                glm::vec3 r2 = matchedRotation2->quat * contactPoint.localAnchor2;

                glm::vec3 deltaSeparation = matchedCorrection2->position + r2 - matchedCorrection1->position - r1;

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
                    bias = glm::max(constraint.biasCoefficient * separation, solverConstants.maxBias);
                    massScale = constraint.massCoefficient;
                    impulseScale = constraint.impulseCoefficient;
                }

                // Relative velocity at contact
                glm::vec3 vr1 = v1 + glm::cross(w1, r1);
                glm::vec3 vr2 = v2 + glm::cross(w2, r2);
                glm::vec3 vr = vr2 - vr1;

                float vn = glm::dot(vr, constraint.normal);

                // Compute normal impulse
                float impulse =
                    -contactPoint.normalMass * massScale * (vn + bias) - impulseScale * contactPoint.normalImpulse;

                // Clamp the accumulated impulse
                float newImpulse = glm::max(contactPoint.normalImpulse + impulse, 0.0F);
                impulse = newImpulse - contactPoint.normalImpulse;
                contactPoint.normalImpulse = newImpulse;

                glm::vec3 p = impulse * constraint.normal;

                v1 -= p * matchedMass1->inverseMass;
                w1 -= matchedInertia1->inverseInertia * glm::cross(r1, p);
                v2 += p * matchedMass2->inverseMass;
                w2 += matchedInertia2->inverseInertia * glm::cross(r2, p);
            }

            glm::vec3 tangent1;
            glm::vec3 tangent2;
            getTangents(v1, v2, constraint.normal, tangent1, tangent2, solverConstants);

            // Friction
            for (PenetrationConstraintPointData& contactPoint : constraint.points)
            {
                glm::vec3 r1 = contactPoint.fixedAnchor1;
                glm::vec3 r2 = contactPoint.fixedAnchor2;

                // Relative velocity at contact
                glm::vec3 vr1 = v1 + glm::cross(w1, r1);
                glm::vec3 vr2 = v2 + glm::cross(w2, r2);
                glm::vec3 vr = vr2 - vr1;

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

                v1 -= p * matchedMass1->inverseMass;
                w1 -= matchedInertia1->inverseInertia * glm::cross(r1, p);
                v2 += p * matchedMass2->inverseMass;
                w2 += matchedInertia2->inverseInertia * glm::cross(r2, p);
            }
        }
        matchedVelocity1->vec = v1;
        matchedAngVelocity1->vec = w1;
        matchedVelocity2->vec = v2;
        matchedAngVelocity2->vec = w2;
    }
}

void cubos::engine::penetrationConstraintPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.relation<PenetrationConstraints>();

    cubos.tag(addPenetrationConstraintTag);
    cubos.tag(penetrationConstraintWarmStartTag);
    cubos.tag(penetrationConstraintSolveTag);
    cubos.tag(penetrationConstraintSolveRelaxTag);
    cubos.tag(penetrationConstraintRestitutionTag);
    cubos.tag(penetrationConstraintCleanTag);

    cubos.system("warm start")
        .tagged(penetrationConstraintWarmStartTag)
        .after(addPenetrationConstraintTag)
        .before(penetrationConstraintSolveTag)
        .tagged(fixedSubstepTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, PenetrationConstraints&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query,
                 const SolverConstants& solverConstants) {
            for (auto [ent1, mass1, inertia1, rotation1, correction1, velocity1, angVelocity1, constraints, ent2, mass2,
                       inertia2, rotation2, correction2, velocity2, angVelocity2] : query)
            {
                const Mass* matchedMass1 = &mass1;
                const Mass* matchedMass2 = &mass2;
                const Inertia* matchedInertia1 = &inertia1;
                const Inertia* matchedInertia2 = &inertia2;
                const Rotation* matchedRotation1 = &rotation1;
                const Rotation* matchedRotation2 = &rotation2;
                const AccumulatedCorrection* matchedCorrection1 = &correction1;
                const AccumulatedCorrection* matchedCorrection2 = &correction2;
                Velocity* matchedVelocity1 = &velocity1;
                Velocity* matchedVelocity2 = &velocity2;
                AngularVelocity* matchedAngVelocity1 = &angVelocity1;
                AngularVelocity* matchedAngVelocity2 = &angVelocity2;

                if (ent1 != constraints.entity)
                {
                    std::swap(matchedMass1, matchedMass2);
                    std::swap(matchedInertia1, matchedInertia2);
                    std::swap(matchedRotation1, matchedRotation2);
                    std::swap(matchedCorrection1, matchedCorrection2);
                    std::swap(matchedVelocity1, matchedVelocity2);
                    std::swap(matchedAngVelocity1, matchedAngVelocity2);
                }

                glm::vec3 v1 = matchedVelocity1->vec;
                glm::vec3 v2 = matchedVelocity2->vec;

                glm::vec3 w1 = matchedAngVelocity1->vec;
                glm::vec3 w2 = matchedAngVelocity2->vec;

                glm::vec3 tangent1;
                glm::vec3 tangent2;

                for (auto& constraint : constraints.penConstraints)
                {
                    getTangents(v1, v2, constraint.normal, tangent1, tangent2, solverConstants);

                    for (PenetrationConstraintPointData& contactPoint : constraint.points)
                    {
                        glm::vec3 r1 = contactPoint.fixedAnchor1;
                        glm::vec3 r2 = contactPoint.fixedAnchor2;

                        glm::vec3 p =
                            solverConstants.warmStartCoefficient * (contactPoint.normalImpulse * constraint.normal) +
                            (contactPoint.frictionImpulse1 * tangent1) + (contactPoint.frictionImpulse2 * tangent2);

                        v1 -= p * matchedMass1->inverseMass;
                        w1 -= matchedInertia1->inverseInertia * glm::cross(r1, p);
                        v2 += p * matchedMass2->inverseMass;
                        w2 += matchedInertia2->inverseInertia * glm::cross(r2, p);
                    }

                    matchedVelocity1->vec = v1;
                    matchedAngVelocity1->vec = w1;
                    matchedVelocity2->vec = v2;
                    matchedAngVelocity2->vec = w2;
                }
            }
        });

    cubos.system("solve contacts bias")
        .tagged(penetrationConstraintSolveTag)
        .tagged(physicsSolveContactTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, PenetrationConstraints&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps,
                 const SolverConstants& solverConstants) {
            solvePenetrationConstraint(query, fixedDeltaTime, substeps, solverConstants, true);
        });

    cubos.system("solve contacts no bias")
        .tagged(penetrationConstraintSolveRelaxTag)
        .tagged(physicsSolveRelaxContactTag)
        .call([](Query<Entity, const Mass&, const Inertia&, const Rotation&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&, PenetrationConstraints&, Entity, const Mass&, const Inertia&, const Rotation&,
                       AccumulatedCorrection&, Velocity&, AngularVelocity&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps,
                 const SolverConstants& solverConstants) {
            solvePenetrationConstraint(query, fixedDeltaTime, substeps, solverConstants, false);
        });

    cubos.system("add restitution")
        .tagged(penetrationConstraintRestitutionTag)
        .after(penetrationConstraintSolveRelaxTag)
        .before(physicsFinalizePositionTag)
        .tagged(fixedStepTag)
        .call([](Query<Entity, const Mass&, const Inertia&, AccumulatedCorrection&, Velocity&, AngularVelocity&,
                       PenetrationConstraints&, Entity, const Mass&, const Inertia&, AccumulatedCorrection&, Velocity&,
                       AngularVelocity&>
                     query,
                 const SolverConstants& solverConstants) {
            for (auto [ent1, mass1, inertia1, correction1, velocity1, angVelocity1, constraints, ent2, mass2, inertia2,
                       correction2, velocity2, angVelocity2] : query)
            {
                for (const auto& constraint : constraints.penConstraints)
                {
                    if (constraint.restitution == solverConstants.minRestitution)
                    {
                        continue;
                    }

                    const Mass* matchedMass1 = &mass1;
                    const Mass* matchedMass2 = &mass2;
                    const Inertia* matchedInertia1 = &inertia1;
                    const Inertia* matchedInertia2 = &inertia2;
                    const AccumulatedCorrection* matchedCorrection1 = &correction1;
                    const AccumulatedCorrection* matchedCorrection2 = &correction2;
                    Velocity* matchedVelocity1 = &velocity1;
                    Velocity* matchedVelocity2 = &velocity2;
                    AngularVelocity* matchedAngVelocity1 = &angVelocity1;
                    AngularVelocity* matchedAngVelocity2 = &angVelocity2;

                    if (ent1 != constraints.entity)
                    {
                        std::swap(matchedMass1, matchedMass2);
                        std::swap(matchedInertia1, matchedInertia2);
                        std::swap(matchedCorrection1, matchedCorrection2);
                        std::swap(matchedVelocity1, matchedVelocity2);
                        std::swap(matchedAngVelocity1, matchedAngVelocity2);
                    }

                    glm::vec3 v1 = matchedVelocity1->vec;
                    glm::vec3 v2 = matchedVelocity2->vec;
                    glm::vec3 w1 = matchedAngVelocity1->vec;
                    glm::vec3 w2 = matchedAngVelocity2->vec;

                    for (auto point : constraint.points)
                    {
                        if (point.normalSpeed > solverConstants.minNormalSpeed ||
                            point.normalImpulse == solverConstants.minNormalImpulse)
                        {
                            continue;
                        }

                        glm::vec3 r1 = point.fixedAnchor1;
                        glm::vec3 r2 = point.fixedAnchor2;

                        // Relative velocity at contact
                        glm::vec3 vr1 = v1 + glm::cross(w1, r1);
                        glm::vec3 vr2 = v2 + glm::cross(w2, r2);
                        glm::vec3 vr = vr2 - vr1;

                        float vn = glm::dot(vr, constraint.normal);

                        // compute normal impulse
                        float impulse = -point.normalMass * (vn + constraint.restitution * point.normalSpeed);

                        // Clamp the accumulated impulse
                        float newImpulse = glm::max(point.normalImpulse + impulse, 0.0F);
                        impulse = newImpulse - point.normalImpulse;
                        point.normalImpulse = newImpulse;

                        // Apply impulse
                        glm::vec3 p = constraint.normal * impulse;
                        v1 -= p * matchedMass1->inverseMass;
                        w1 -= matchedInertia1->inverseInertia * glm::cross(r1, p);
                        v2 += p * matchedMass2->inverseMass;
                        w2 += matchedInertia2->inverseInertia * glm::cross(r2, p);
                    }

                    matchedVelocity1->vec = v1;
                    matchedAngVelocity1->vec = w1;
                    matchedVelocity2->vec = v2;
                    matchedAngVelocity2->vec = w2;
                }
            }
        });

    cubos.system("add penetration constraint pair")
        .tagged(addPenetrationConstraintTag)
        .tagged(physicsPrepareSolveTag)
        .call([](Commands cmds,
                 Query<Entity, const Mass&, const Inertia&, const CenterOfMass&, const Rotation&, const Velocity&,
                       const AngularVelocity&, const PhysicsMaterial&, const CollidingWith&, Entity, const Mass&,
                       const Inertia&, const CenterOfMass&, const Rotation&, const Velocity&, const AngularVelocity&,
                       const PhysicsMaterial&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps,
                 const SolverConstants& solverConstants) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;
            float contactHertz = glm::min(solverConstants.minContactHertz, 0.25F * (1.0F / subDeltaTime));

            for (auto [ent1, mass1, inertia1, centerOfMass1, rotation1, velocity1, angVelocity1, material1,
                       collidingWith, ent2, mass2, inertia2, centerOfMass2, rotation2, velocity2, angVelocity2,
                       material2] : query)
            {
                const Mass* matchedMass1 = &mass1;
                const Mass* matchedMass2 = &mass2;
                const Inertia* matchedInertia1 = &inertia1;
                const Inertia* matchedInertia2 = &inertia2;
                const CenterOfMass* matchedCenterOfMass1 = &centerOfMass1;
                const CenterOfMass* matchedCenterOfMass2 = &centerOfMass2;
                const Rotation* matchedRotation1 = &rotation1;
                const Rotation* matchedRotation2 = &rotation2;
                const Velocity* matchedVelocity1 = &velocity1;
                const Velocity* matchedVelocity2 = &velocity2;
                const AngularVelocity* matchedAngVelocity1 = &angVelocity1;
                const AngularVelocity* matchedAngVelocity2 = &angVelocity2;
                const PhysicsMaterial* matchedMaterial1 = &material1;
                const PhysicsMaterial* matchedMaterial2 = &material2;

                if (ent1 != collidingWith.entity)
                {
                    std::swap(matchedMass1, matchedMass2);
                    std::swap(matchedInertia1, matchedInertia2);
                    std::swap(matchedCenterOfMass1, matchedCenterOfMass2);
                    std::swap(matchedRotation1, matchedRotation2);
                    std::swap(matchedVelocity1, matchedVelocity2);
                    std::swap(matchedAngVelocity1, matchedAngVelocity2);
                    std::swap(matchedMaterial1, matchedMaterial2);
                }

                std::vector<PenetrationConstraint> penConstraints;
                for (const auto& manifold : collidingWith.manifolds)
                {
                    glm::vec3 tangent1;
                    glm::vec3 tangent2;
                    getTangents(matchedVelocity1->vec, matchedVelocity2->vec, manifold.normal, tangent1, tangent2,
                                solverConstants);

                    std::vector<PenetrationConstraintPointData> points;
                    for (auto point : manifold.points)
                    {
                        auto pointData = PenetrationConstraintPointData{};

                        pointData.normalImpulse = point.normalImpulse;
                        pointData.frictionImpulse1 = point.frictionImpulse1;
                        pointData.frictionImpulse2 = point.frictionImpulse2;

                        pointData.localAnchor1 = point.localOn1 - matchedCenterOfMass1->vec;
                        pointData.localAnchor2 = point.localOn2 - matchedCenterOfMass2->vec;

                        glm::vec3 r1 = matchedRotation1->quat * pointData.localAnchor1;
                        glm::vec3 r2 = matchedRotation2->quat * pointData.localAnchor2;

                        pointData.fixedAnchor1 = r1;
                        pointData.fixedAnchor2 = r2;

                        pointData.initialSeparation = -point.penetration - glm::dot(r2 - r1, manifold.normal);

                        // Relative velocity at contact
                        glm::vec3 vr1 = matchedVelocity1->vec + glm::cross(matchedAngVelocity1->vec, r1);
                        glm::vec3 vr2 = matchedVelocity2->vec + glm::cross(matchedAngVelocity2->vec, r2);
                        glm::vec3 vr = vr2 - vr1;

                        pointData.normalSpeed = glm::dot(vr, manifold.normal);

                        // normal mass
                        glm::vec3 rn1 = glm::cross(r1, manifold.normal);
                        glm::vec3 rn2 = glm::cross(r2, manifold.normal);
                        float kNormal = mass1.inverseMass + mass2.inverseMass +
                                        glm::dot(matchedInertia1->inverseInertia * rn1, rn1) +
                                        glm::dot(matchedInertia2->inverseInertia * rn2, rn2);
                        pointData.normalMass = kNormal > solverConstants.minKNormal ? 1.0F / kNormal : 0.0F;

                        // friction mass
                        glm::vec3 rt11 = glm::cross(r1, tangent1);
                        glm::vec3 rt12 = glm::cross(r2, tangent1);
                        glm::vec3 rt21 = glm::cross(r1, tangent2);
                        glm::vec3 rt22 = glm::cross(r2, tangent2);

                        // Multiply by the inverse inertia early to reuse the values
                        glm::vec3 i1Rt11 = matchedInertia1->inverseInertia * rt11;
                        glm::vec3 i2Rt12 = matchedInertia2->inverseInertia * rt12;
                        glm::vec3 i1Rt21 = matchedInertia1->inverseInertia * rt21;
                        glm::vec3 i2Rt22 = matchedInertia2->inverseInertia * rt22;

                        float kFriction1 =
                            mass1.inverseMass + mass2.inverseMass + glm::dot(i1Rt11, rt11) + glm::dot(i2Rt12, rt12);
                        float kFriction2 =
                            mass1.inverseMass + mass2.inverseMass + glm::dot(i1Rt21, rt21) + glm::dot(i2Rt22, rt22);

                        /// TODO: these could be an array in the point
                        pointData.frictionMass1 = kFriction1 > solverConstants.minKFriction ? 1.0F / kFriction1 : 0.0F;
                        pointData.frictionMass2 = kFriction2 > solverConstants.minKFriction ? 1.0F / kFriction2 : 0.0F;

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

                    penConstraints.push_back(PenetrationConstraint{.normal = manifold.normal,
                                                                   .friction = friction,
                                                                   .restitution = restitution,
                                                                   .points = points,
                                                                   .biasCoefficient = biasCoefficient,
                                                                   .impulseCoefficient = impulseCoefficient,
                                                                   .massCoefficient = massCoefficient});
                }
                cmds.relate(ent1, ent2,
                            PenetrationConstraints{.entity = collidingWith.entity, .penConstraints = penConstraints});
            }
        });

    cubos.system("store impulses and clean penetration constraint pairs")
        .tagged(physicsFinalizePositionTag)
        .call([](Commands cmds, Query<Entity, CollidingWith&, Entity> cQuery,
                 Query<Entity, PenetrationConstraints&, Entity> pQuery) {
            for (auto [entity, collidingWith, other] : cQuery)
            {
                if (auto match = pQuery.pin(0, entity).pin(1, other).first())
                {
                    auto [entity, constraints, other] = *match;

                    for (size_t i = 0; i < collidingWith.manifolds.size(); i++)
                    {
                        for (size_t j = 0; j < collidingWith.manifolds[i].points.size(); j++)
                        {
                            collidingWith.manifolds[i].points[j].normalImpulse =
                                constraints.penConstraints[i].points[j].normalImpulse;
                            collidingWith.manifolds[i].points[j].frictionImpulse1 =
                                constraints.penConstraints[i].points[j].frictionImpulse1;
                            collidingWith.manifolds[i].points[j].frictionImpulse2 =
                                constraints.penConstraints[i].points[j].frictionImpulse2;
                        }
                    }
                }
                cmds.unrelate<PenetrationConstraints>(entity, other);
            }
        });
}
