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
    (void)query;
    (void)collisions;
}

void sweep(Write<BroadPhaseCollisions> collisions)
{
    (void)collisions;
}

void findPairs(Write<BroadPhaseCollisions> collisions)
{
    (void)collisions;
}