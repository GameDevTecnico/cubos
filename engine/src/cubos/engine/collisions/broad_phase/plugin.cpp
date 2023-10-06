
#include "plugin.hpp"

#include <cubos/engine/collisions/broad_phase/candidates.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/transform/local_to_world.hpp>

#include "sweep_and_prune.hpp"

using cubos::core::ecs::OptRead;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

/// @brief Tracks all new colliders.
void trackNewColliders(Query<Read<Collider>> query, Write<BroadPhaseSweepAndPrune> sweepAndPrune)
{
    for (auto [entity, collider] : query)
    {
        if (collider->fresh == 0)
        {
            sweepAndPrune->addEntity(entity);
        }
    }
}

/// @brief Updates the AABBs of all colliders.
void updateAABBs(Query<Read<LocalToWorld>, Write<Collider>> query)
{
    for (auto [entity, localToWorld, collider] : query)
    {
        // Get the 4 points of the collider.
        glm::vec3 corners[4];
        collider->localAABB.box().corners4(corners);

        // Pack the 3 points of the collider into a matrix.
        auto points = glm::mat4{glm::vec4{corners[0], 1.0F}, glm::vec4{corners[1], 1.0F}, glm::vec4{corners[2], 1.0F},
                                glm::vec4{corners[3], 1.0F}};

        // Transforms collider space to world space.
        auto transform = localToWorld->mat * collider->transform;

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
        max += glm::vec3{collider->margin};

        // Set the AABB.
        collider->worldAABB.min(translation - max);
        collider->worldAABB.max(translation + max);
    };
}

/// @brief Updates the sweep markers of all colliders.
void updateMarkers(Query<Read<Collider>> query, Write<BroadPhaseSweepAndPrune> sweepAndPrune)
{
    // TODO: This is parallelizable.
    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        // TODO: Should use insert sort to leverage spatial coherence.
        std::sort(sweepAndPrune->markersPerAxis[axis].begin(), sweepAndPrune->markersPerAxis[axis].end(),
                  [axis, &query](const BroadPhaseSweepAndPrune::SweepMarker& a,
                                 const BroadPhaseSweepAndPrune::SweepMarker& b) {
                      auto [aCollider] = query[a.entity].value();
                      auto [bCollider] = query[b.entity].value();
                      auto aPos = a.isMin ? aCollider->worldAABB.min() : aCollider->worldAABB.max();
                      auto bPos = b.isMin ? bCollider->worldAABB.min() : bCollider->worldAABB.max();
                      return aPos[axis] < bPos[axis];
                  });
    }
}

/// @brief Performs a sweep of all colliders.
void sweep(Write<BroadPhaseSweepAndPrune> sweepAndPrune)
{
    // TODO: This is parallelizable.
    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        CUBOS_ASSERT(sweepAndPrune->activePerAxis[axis].empty(), "Last sweep entered an entity but never exited");

        sweepAndPrune->sweepOverlapMaps[axis].clear();

        for (auto& marker : sweepAndPrune->markersPerAxis[axis])
        {
            if (marker.isMin)
            {
                for (const auto& other : sweepAndPrune->activePerAxis[axis])
                {
                    sweepAndPrune->sweepOverlapMaps[axis][marker.entity].push_back(other);
                }

                sweepAndPrune->activePerAxis[axis].insert(marker.entity);
            }
            else
            {
                sweepAndPrune->activePerAxis[axis].erase(marker.entity);
            }
        }
    }
}

BroadPhaseCandidates::CollisionType getCollisionType(bool box, bool capsule)
{
    if (box && capsule)
    {
        return BroadPhaseCandidates::CollisionType::BoxCapsule;
    }

    if (box)
    {
        return BroadPhaseCandidates::CollisionType::BoxBox;
    }

    return BroadPhaseCandidates::CollisionType::CapsuleCapsule;
}

/// @brief Finds all pairs of colliders which may be colliding.
///
/// @details
/// TODO: This query is disgusting. We need a way to find if a component is present without reading it.
/// Maybe something like Commands but for reads?
void findPairs(Query<OptRead<BoxCollisionShape>, OptRead<CapsuleCollisionShape>, Read<Collider>> query,
               Read<BroadPhaseSweepAndPrune> sweepAndPrune, Write<BroadPhaseCandidates> candidates)
{
    candidates->clearCandidates();

    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        for (const auto& [entity, overlaps] : sweepAndPrune->sweepOverlapMaps[axis])
        {
            auto [box, capsule, collider] = query[entity].value();
            for (const auto& other : overlaps)
            {
                auto [otherBox, otherCapsule, otherCollider] = query[other].value();

                // TODO: Should this be inside the if statement?
                auto type = getCollisionType(box || otherBox, capsule || otherCapsule);

                switch (axis)
                {
                case 0: // X
                    if (collider->worldAABB.overlapsY(otherCollider->worldAABB) &&
                        collider->worldAABB.overlapsZ(otherCollider->worldAABB))
                    {
                        candidates->addCandidate(type, {entity, other});
                    }
                    break;
                case 1: // Y
                    if (collider->worldAABB.overlapsX(otherCollider->worldAABB) &&
                        collider->worldAABB.overlapsZ(otherCollider->worldAABB))
                    {
                        candidates->addCandidate(type, {entity, other});
                    }
                    break;
                case 2: // Z
                    if (collider->worldAABB.overlapsX(otherCollider->worldAABB) &&
                        collider->worldAABB.overlapsY(otherCollider->worldAABB))
                    {
                        candidates->addCandidate(type, {entity, other});
                    }
                    break;
                }
            }
        }
    }
}

void cubos::engine::broadPhaseCollisionsPlugin(Cubos& cubos)
{
    // FIXME: Is it ok not to add resources from the general plugin?

    cubos.addResource<BroadPhaseCandidates>();
    cubos.addResource<BroadPhaseSweepAndPrune>();

    cubos.system(trackNewColliders).tagged("cubos.collisions.aabb.setup");

    cubos.system(updateAABBs)
        .tagged("cubos.collisions.aabb.update")
        .after("cubos.collisions.setup")
        .after("cubos.collisions.aabb.setup")
        .after("cubos.transform.update");

    cubos.system(updateMarkers).tagged("cubos.collisions.broad.markers").after("cubos.collisions.aabb.update");
    cubos.system(sweep).tagged("cubos.collisions.broad.sweep").after("cubos.collisions.broad.markers");
    cubos.system(findPairs).tagged("cubos.collisions.broad").after("cubos.collisions.broad.sweep");
}