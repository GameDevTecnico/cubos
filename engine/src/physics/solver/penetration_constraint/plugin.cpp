#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/components/accumulated_correction.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../../fixed_substep/plugin.hpp"
#include "../utils.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::addPenetrationConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintCleanTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintSolveTag);

glm::vec3 applyCorrectionToPosition(float inverseMass, glm::vec3 position, glm::vec3 correction)
{
    if (inverseMass <= 0.0F)
    {
        return position;
    }
    return position + correction; // lagrange * correction * inverseMass
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
    cubos.tag(penetrationConstraintCleanTag);
    cubos.tag(penetrationConstraintSolveTag);

    // collision solving
    cubos.system("solve penetration constraints")
        .tagged(penetrationConstraintSolveTag)
        .tagged(physicsSolveTag)
        .call([](Query<Entity, const Mass&, Position&, AccumulatedCorrection&, PenetrationConstraint&, Entity,
                       const Mass&, Position&, AccumulatedCorrection&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            for (auto [ent1, mass1, position1, correction1, constraint, ent2, mass2, position2, correction2] : query)
            {
                float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

                // We will want to have contact points here, so we can easily calculate the penetration when we get
                // rigid bodies
                auto penetration = constraint.penetration;

                float distForOgPenetration = glm::abs(glm::dot(
                    constraint.entity1OriginalPosition - constraint.entity2OriginalPosition, constraint.normal));
                float distForNowPenetration = glm::abs(glm::dot(position1.vec - position2.vec, constraint.normal));

                penetration += distForOgPenetration - distForNowPenetration;

                glm::vec3 gradients[2] = {constraint.normal, -constraint.normal};
                float inverseMasses[2] = {mass1.inverseMass, mass2.inverseMass};

                // We use max to cap the distance that we can move the objects in a substep.
                // We do this to prevent the velocity calculated after the correction from being too high making the
                // simulation very explosive.
                // The solution proposed in the papers is to choose a maximum velocity for a
                // substep, however we couldn't get the simulation to stabilize like that so we opted so for a cap based
                // on the current penetration between the objects.
                float max = glm::max(penetration / (float)substeps.value, 0.0F);
                float c = penetration - glm::max(penetration - max, 0.0F);
                if (c <= 0.0F)
                {
                    continue;
                }

                float lagrangeUpdate =
                    getLagrangeMultiplierUpdate(c, inverseMasses, gradients, 2, constraint.compliance, subDeltaTime);

                glm::vec3 positionUpdate = lagrangeUpdate * constraint.normal;

                if (ent1 == constraint.entity)
                {
                    correction1.vec += positionUpdate * mass1.inverseMass;
                    correction2.vec -= positionUpdate * mass2.inverseMass;
                    position1.vec = applyCorrectionToPosition(mass1.inverseMass, position1.vec, correction1.vec);
                    position2.vec = applyCorrectionToPosition(mass2.inverseMass, position2.vec, correction2.vec);
                }
                else
                {
                    correction1.vec -= positionUpdate * mass1.inverseMass;
                    correction2.vec += positionUpdate * mass2.inverseMass;
                    position1.vec = applyCorrectionToPosition(mass1.inverseMass, position1.vec, correction1.vec);
                    position2.vec = applyCorrectionToPosition(mass2.inverseMass, position2.vec, correction2.vec);
                }
                correction1.vec = glm::vec3(0, 0, 0);
                correction2.vec = glm::vec3(0, 0, 0);
            }
        });

    cubos.system("add penetration constraint pair")
        .tagged(addPenetrationConstraintTag)
        .after(collisionsTag)
        .before(penetrationConstraintSolveTag)
        .tagged(fixedStepTag)
        .call([](Commands cmds, Query<Entity, const CollidingWith&, Entity> query) {
            for (auto [ent1, collidingWith, ent2] : query)
            {
                cmds.relate(ent1, ent2,
                            PenetrationConstraint{.entity = ent1,
                                                  .entity1OriginalPosition = collidingWith.entity1OriginalPosition,
                                                  .entity2OriginalPosition = collidingWith.entity2OriginalPosition,
                                                  .penetration = collidingWith.penetration,
                                                  .normal = collidingWith.normal});
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
