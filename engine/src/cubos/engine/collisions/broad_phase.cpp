#include "broad_phase.hpp"

#include <cubos/core/log.hpp>

void updateBoxAABBs(Query<Read<LocalToWorld>, Read<BoxCollider>, Write<ColliderAABB>> query)
{
    (void)query;
}

void updateCapsuleAABBs(Query<Read<LocalToWorld>, Read<CapsuleCollider>, Write<ColliderAABB>> query,
                        Write<BroadPhaseCollisions> collisions)
{
    (void)query;
    (void)collisions;
}

void updateSimplexAABBs(Query<Read<LocalToWorld>, Read<SimplexCollider>, Write<ColliderAABB>> query,
                        Write<BroadPhaseCollisions> collisions)
{
    (void)query;
    (void)collisions;
}

void updateMarkers(Query<Read<ColliderAABB>> query, Write<BroadPhaseCollisions> collisions)
{
    // TODO: This is parallelizable.
    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        // TODO: Should use insert sort to leverage spatial coherence.
        std::sort(
            collisions->markersPerAxis[axis].begin(), collisions->markersPerAxis[axis].end(),
            [axis, &query](const BroadPhaseCollisions::SweepMarker& a, const BroadPhaseCollisions::SweepMarker& b) {
                auto [aAABB] = query[a.entity].value();
                auto [bAABB] = query[b.entity].value();
                auto aPos = a.isMin ? aAABB->min() : aAABB->max();
                auto bPos = b.isMin ? bAABB->min() : bAABB->max();
                return aPos[axis] < bPos[axis];
            });
    }
}

void sweep(Write<BroadPhaseCollisions> collisions)
{
    // TODO: This is parallelizable.
    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        CUBOS_ASSERT(collisions->activePerAxis[axis].empty(), "Last sweep entered an entity but never exited");

        collisions->sweepOverlapMaps[axis].clear();

        for (auto& marker : collisions->markersPerAxis[axis])
        {
            if (marker.isMin)
            {
                for (auto& other : collisions->activePerAxis[axis])
                {
                    collisions->sweepOverlapMaps[axis][marker.entity].push_back(other);
                }

                collisions->activePerAxis[axis].insert(marker.entity);
            }
            else
            {
                collisions->activePerAxis[axis].erase(marker.entity);
            }
        }
    }
}

void findPairs(Write<BroadPhaseCollisions> collisions)
{
    (void)collisions;
}