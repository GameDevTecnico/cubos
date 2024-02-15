#include "plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/transform/local_to_world.hpp>

#include "sweep_and_prune.hpp"

void cubos::engine::broadPhaseCollisionsPlugin(Cubos& cubos)
{
    cubos.addResource<BroadPhaseSweepAndPrune>();
    cubos.addRelation<PotentiallyCollidingWith>();

    cubos.system("detect new colliders")
        .tagged("cubos.collisions.aabb.setup")
        .call([](Query<Entity, const Collider&> query, BroadPhaseSweepAndPrune& sweepAndPrune) {
            for (auto [entity, collider] : query)
            {
                if (collider.fresh == 0)
                {
                    sweepAndPrune.addEntity(entity);
                }
            }
        });

    cubos.system("update collider AABBs")
        .tagged("cubos.collisions.aabb.update")
        .after("cubos.collisions.setup")
        .after("cubos.collisions.aabb.setup")
        .after("cubos.transform.update")
        .call([](Query<const LocalToWorld&, Collider&> query) {
            for (auto [localToWorld, collider] : query)
            {
                // Get the 4 points of the collider.
                glm::vec3 corners[4];
                collider.localAABB.box().corners4(corners);

                // Pack the 3 points of the collider into a matrix.
                auto points = glm::mat4{glm::vec4{corners[0], 1.0F}, glm::vec4{corners[1], 1.0F},
                                        glm::vec4{corners[2], 1.0F}, glm::vec4{corners[3], 1.0F}};

                // Transforms collider space to world space.
                auto transform = localToWorld.mat * collider.transform;

                // Only want scale and rotation, extract translation and remove it.
                auto translation = glm::vec3{transform[3]};
                transform[3] = glm::vec4{0.0F, 0.0F, 0.0F, 1.0F};

                // Rotate and scale corners.
                auto rotatedCorners = glm::mat4x3{transform * points};

                // Get the max of the rotated corners.
                auto max = glm::max(glm::abs(rotatedCorners[0]), glm::abs(rotatedCorners[1]));
                max = glm::max(max, glm::abs(rotatedCorners[2]));
                max = glm::max(max, glm::abs(rotatedCorners[3]));

                // Add the collider's margin.
                max += glm::vec3{collider.margin};

                // Set the AABB.
                collider.worldAABB.min(translation - max);
                collider.worldAABB.max(translation + max);
            };
        });

    cubos.system("update sweep and prune markers")
        .tagged("cubos.collisions.broad.markers")
        .after("cubos.collisions.aabb.update")
        .call([](Query<const Collider&> query, BroadPhaseSweepAndPrune& sweepAndPrune) {
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
                                  auto [aCollider] = *aMatch;
                                  auto [bCollider] = *bMatch;
                                  auto aPos = a.isMin ? aCollider.worldAABB.min() : aCollider.worldAABB.max();
                                  auto bPos = b.isMin ? bCollider.worldAABB.min() : bCollider.worldAABB.max();
                                  return aPos[axis] < bPos[axis];
                              }
                              return true;
                          });
            }
        });

    cubos.system("collisions sweep")
        .tagged("cubos.collisions.broad.sweep")
        .after("cubos.collisions.broad.markers")
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
        .tagged("cubos.collisions.broad.clean")
        .before("cubos.collisions.broad")
        .call([](Commands cmds, Query<Entity, PotentiallyCollidingWith&, Entity> query) {
            for (auto [entity, collidingWith, other] : query)
            {
                cmds.unrelate<PotentiallyCollidingWith>(entity, other);
            }
        });

    cubos.system("create PotentiallyCollidingWith relations")
        .tagged("cubos.collisions.broad")
        .after("cubos.collisions.broad.sweep")
        .call([](Commands cmds, Query<const Collider&> query, const BroadPhaseSweepAndPrune& sweepAndPrune) {
            for (glm::length_t axis = 0; axis < 3; axis++)
            {
                for (const auto& [entity, overlaps] : sweepAndPrune.sweepOverlapMaps[axis])
                {
                    auto match = query.at(entity);
                    if (!match)
                    {
                        continue;
                    }
                    auto [collider] = *match;

                    for (const auto& other : overlaps)
                    {
                        auto otherMatch = query.at(other);
                        if (!otherMatch)
                        {
                            continue;
                        }
                        auto [otherCollider] = *otherMatch;

                        switch (axis)
                        {
                        case 0: // X
                            if (collider.worldAABB.overlapsY(otherCollider.worldAABB) &&
                                collider.worldAABB.overlapsZ(otherCollider.worldAABB))
                            {
                                cmds.relate(entity, other, PotentiallyCollidingWith{});
                            }
                            break;
                        case 1: // Y
                            if (collider.worldAABB.overlapsX(otherCollider.worldAABB) &&
                                collider.worldAABB.overlapsZ(otherCollider.worldAABB))
                            {
                                cmds.relate(entity, other, PotentiallyCollidingWith{});
                            }
                            break;
                        case 2: // Z
                            if (collider.worldAABB.overlapsX(otherCollider.worldAABB) &&
                                collider.worldAABB.overlapsY(otherCollider.worldAABB))
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
