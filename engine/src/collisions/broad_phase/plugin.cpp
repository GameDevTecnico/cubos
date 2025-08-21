#include "plugin.hpp"
#include <algorithm>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/collider_aabb.hpp>
#include <cubos/engine/collisions/collision_layers.hpp>
#include <cubos/engine/collisions/collision_mask.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../interface/plugin.hpp"
#include "sweep_and_prune.hpp"

CUBOS_DEFINE_TAG(cubos::engine::collisionsAABBUpdateTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsBroadMarkersTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsBroadSweepTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsBroadCleanTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsBroadTag);

void cubos::engine::broadPhaseCollisionsPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(interfaceCollisionsPlugin);

    cubos.relation<PotentiallyCollidingWith>();

    cubos.resource<BroadPhaseSweepAndPrune>();

    cubos.tag(collisionsAABBUpdateTag).tagged(fixedStepTag);
    cubos.tag(collisionsBroadMarkersTag).tagged(fixedStepTag);
    cubos.tag(collisionsBroadSweepTag).tagged(fixedStepTag);
    cubos.tag(collisionsBroadCleanTag).tagged(fixedStepTag);
    cubos.tag(collisionsBroadTag).tagged(fixedStepTag);

    cubos.observer("add new Colliders to SweepAndPrune")
        .onAdd<ColliderAABB>()
        .call([](Query<Entity> query, BroadPhaseSweepAndPrune& sweepAndPrune) {
            for (auto [entity] : query)
            {
                sweepAndPrune.addEntity(entity);
            }
        });

    cubos.observer("remove old Colliders from SweepAndPrune")
        .onRemove<ColliderAABB>()
        .call([](Query<Entity> query, BroadPhaseSweepAndPrune& sweepAndPrune) {
            for (auto [entity] : query)
            {
                sweepAndPrune.removeEntity(entity);
            }
        });

    cubos.system("update collider AABBs")
        .tagged(collisionsAABBUpdateTag)
        .after(transformUpdateTag)
        .call([](Query<const LocalToWorld&, ColliderAABB&> query) {
            for (auto [localToWorld, colliderAABB] : query)
            {
                colliderAABB.worldAABB = core::geom::AABB::fromOBB(colliderAABB.localAABB.box(), localToWorld.mat);
            };
        });

    cubos.system("update sweep and prune markers")
        .tagged(collisionsBroadMarkersTag)
        .after(collisionsAABBUpdateTag)
        .call([](Query<const ColliderAABB&> query, BroadPhaseSweepAndPrune& sweepAndPrune) {
            // TODO: This is parallelizable.
            for (glm::length_t axis = 0; axis < 3; axis++)
            {
                // TODO: Should use insert sort to leverage spatial coherence.
                std::sort(sweepAndPrune.markersPerAxis[axis].begin(), sweepAndPrune.markersPerAxis[axis].end(),
                          [axis, &query](const BroadPhaseSweepAndPrune::SweepMarker& a,
                                         const BroadPhaseSweepAndPrune::SweepMarker& b) {
                              auto aMatch = query.at(a.entity);
                              auto bMatch = query.at(b.entity);
                              if (aMatch && bMatch)
                              {
                                  auto [aColliderAABB] = *aMatch;
                                  auto [bColliderAABB] = *bMatch;
                                  auto aPos = a.isMin ? aColliderAABB.worldAABB.min() : aColliderAABB.worldAABB.max();
                                  auto bPos = b.isMin ? bColliderAABB.worldAABB.min() : bColliderAABB.worldAABB.max();
                                  return aPos[axis] < bPos[axis];
                              }
                              return true;
                          });
            }
        });

    cubos.system("collisions sweep")
        .tagged(collisionsBroadSweepTag)
        .after(collisionsBroadMarkersTag)
        .call([](BroadPhaseSweepAndPrune& sweepAndPrune) {
            // TODO: This is parallelizable.
            for (glm::length_t axis = 0; axis < 3; axis++)
            {
                sweepAndPrune.activePerAxis[axis].clear();
                sweepAndPrune.sweepOverlapMaps[axis].clear();

                for (auto& marker : sweepAndPrune.markersPerAxis[axis])
                {
                    if (marker.isMin)
                    {
                        for (const auto& other : sweepAndPrune.activePerAxis[axis])
                        {
                            sweepAndPrune.sweepOverlapMaps[axis][marker.entity].push_back(other);
                        }

                        sweepAndPrune.activePerAxis[axis].insert(marker.entity);
                    }
                    else
                    {
                        sweepAndPrune.activePerAxis[axis].erase(marker.entity);
                    }
                }
            }
        });

    cubos.system("clean PotentiallyCollidingWith relations")
        .tagged(collisionsBroadCleanTag)
        .before(collisionsBroadTag)
        .call([](Commands cmds, Query<Entity, PotentiallyCollidingWith&, Entity> query) {
            for (auto [entity, collidingWith, other] : query)
            {
                cmds.unrelate<PotentiallyCollidingWith>(entity, other);
            }
        });

    cubos.system("create PotentiallyCollidingWith relations")
        .tagged(collisionsBroadTag)
        .after(collisionsBroadSweepTag)
        .call([](Commands cmds,
                 Query<const ColliderAABB&, const CollisionLayers&, const CollisionMask&, const Collider&> query,
                 const BroadPhaseSweepAndPrune& sweepAndPrune) {
            for (glm::length_t axis = 0; axis < 3; axis++)
            {
                for (const auto& [entity, overlaps] : sweepAndPrune.sweepOverlapMaps[axis])
                {
                    auto match = query.at(entity);
                    if (!match)
                    {
                        continue;
                    }
                    auto [colliderAABB, layers, mask, collider] = *match;

                    for (const auto& other : overlaps)
                    {
                        auto otherMatch = query.at(other);
                        if (!otherMatch)
                        {
                            continue;
                        }
                        auto [otherColliderAABB, otherLayers, otherMask, otherCollider] = *otherMatch;

                        // Check is shapes are active and if are not static
                        if ((!collider.isActive || !otherCollider.isActive) ||
                            (collider.isStatic && otherCollider.isStatic))
                        {
                            continue;
                        }
                        if (((layers.value & otherMask.value) == 0U) && ((otherLayers.value & mask.value) == 0U))
                        {
                            continue;
                        }

                        switch (axis)
                        {
                        case 0: // X
                            if (colliderAABB.worldAABB.overlapsY(otherColliderAABB.worldAABB) &&
                                colliderAABB.worldAABB.overlapsZ(otherColliderAABB.worldAABB))
                            {
                                cmds.relate(entity, other, PotentiallyCollidingWith{});
                            }
                            break;
                        case 1: // Y
                            if (colliderAABB.worldAABB.overlapsX(otherColliderAABB.worldAABB) &&
                                colliderAABB.worldAABB.overlapsZ(otherColliderAABB.worldAABB))
                            {
                                cmds.relate(entity, other, PotentiallyCollidingWith{});
                            }
                            break;
                        case 2: // Z
                            if (colliderAABB.worldAABB.overlapsX(otherColliderAABB.worldAABB) &&
                                colliderAABB.worldAABB.overlapsY(otherColliderAABB.worldAABB))
                            {
                                cmds.relate(entity, other, PotentiallyCollidingWith{});
                            }
                            break;
                        }
                    }
                }
            }
        });
}
