#include "../../../collisions/narrow_phase/plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/components/accumulated_correction.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../utils.hpp"
#include "plugin.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::addPenetrationConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintCleanTag);
CUBOS_DEFINE_TAG(cubos::engine::penetrationConstraintSolveTag);

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
        .after(collisionsNarrowTag)
        .tagged(physicsSolveTag)
        .call([](Query<Entity, const Mass&, const Position&, AccumulatedCorrection&, PenetrationConstraint&, Entity,
                       const Mass&, const Position&, AccumulatedCorrection&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            for (auto [ent1, mass1, position1, correction1, constraint, ent2, mass2, position2, correction2] : query)
            {
                float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

                // We will want to have contact points here, so we can easily calculate the penetration when we get
                // rigid bodies
                auto penetration = constraint.penetration;

                auto differencePenetration1 =
                    glm::dot(position1.vec - constraint.entity1OriginalPosition, constraint.normal);
                auto differencePenetration2 =
                    glm::dot(position2.vec - constraint.entity2OriginalPosition, constraint.normal);

                if (ent1 == constraint.entity)
                {
                    penetration = penetration + differencePenetration1 - differencePenetration2;
                }
                else
                {
                    penetration = penetration - differencePenetration1 + differencePenetration2;
                }

                glm::vec3 gradients[2] = {constraint.normal, -constraint.normal};
                float inverseMasses[2] = {mass1.inverseMass, mass2.inverseMass};

                float lagrangeUpdate =
                    getLagrangeMultiplierUpdate(penetration, constraint.lagrangeMultiplier, inverseMasses, gradients, 2,
                                                constraint.compliance, subDeltaTime);

                glm::vec3 positionUpdate = lagrangeUpdate * constraint.normal;

                if (ent1 == constraint.entity)
                {
                    correction1.vec += positionUpdate * mass1.inverseMass;
                    correction2.vec -= positionUpdate * mass2.inverseMass;
                }
                else
                {
                    correction1.vec -= positionUpdate * mass1.inverseMass;
                    correction2.vec += positionUpdate * mass2.inverseMass;
                }

                constraint.lagrangeMultiplier += lagrangeUpdate;
            }
        });

    cubos.system("clear penetration constraint lagrange multiplier")
        .tagged(physicsSimulationClearForcesTag)
        .after(physicsSimulationSubstepsUpdateVelocityTag)
        .tagged(fixedStepTag)
        .call([](Query<PenetrationConstraint&> query) {
            for (auto [constraint] : query)
            {
                constraint.lagrangeMultiplier = 0.0F;
            }
        });

    cubos.system("add penetration constraint pair")
        .tagged(addPenetrationConstraintTag)
        .after(collisionsNarrowTag)
        .before(penetrationConstraintSolveTag)
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
        .before(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, PenetrationConstraint&, Entity> query) {
            for (auto [entity, constraint, other] : query)
            {
                cmds.unrelate<PenetrationConstraint>(entity, other);
            }
        });
}
