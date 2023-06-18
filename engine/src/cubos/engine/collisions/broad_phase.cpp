#include "broad_phase.hpp"

#include <cubos/core/log.hpp>

using CollisionType = BroadPhaseCollisions::CollisionType;

void updateBoxAABBs(Query<Read<LocalToWorld>, Read<BoxCollider>, Write<ColliderAABB>> query)
{
    for (auto [entity, localToWorld, collider, aabb] : query)
    {
        // Get the 4 points of the collider.
        glm::vec3 corners[4];
        collider->shape.corners4(corners);

        // Pack the 3 points of the collider into a matrix.
        auto points = glm::mat4{glm::vec4{corners[0], 1.0F}, glm::vec4{corners[1], 1.0F}, glm::vec4{corners[2], 1.0F},
                                glm::vec4{corners[3], 1.0F}};

        // Transforms collider space to world space.
        auto transform = localToWorld->mat * collider->transform;

        // Only want scale and rotation, extract translation and remove it.
        auto translation = glm::vec3{transform[3]};
        transform[3] = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};

        // Rotate and scale corners.
        auto rotatedCorners = glm::mat4x3{transform * points};

        // Get the max of the rotated corners.
        auto max = glm::max(glm::abs(rotatedCorners[0]), glm::abs(rotatedCorners[1]));
        max = glm::max(max, glm::abs(rotatedCorners[2]));
        max = glm::max(max, glm::abs(rotatedCorners[3]));

        // Add the collider's margin.
        max += glm::vec3{collider->margin};

        // Set the AABB.
        aabb->max = translation + max;
        aabb->min = translation - max;
    }
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
                auto aPos = a.isMin ? aAABB->min : aAABB->max;
                auto bPos = b.isMin ? bAABB->min : bAABB->max;
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

CollisionType getCollisionType(bool box, bool capsule, bool plane, bool simplex)
{
    if (box && capsule)
        return CollisionType::BoxCapsule;
    if (box && plane)
        return CollisionType::BoxPlane;
    if (box && simplex)
        return CollisionType::BoxSimplex;
    if (box)
        return CollisionType::BoxBox;
    if (capsule && plane)
        return CollisionType::CapsulePlane;
    if (capsule && simplex)
        return CollisionType::CapsuleSimplex;
    if (capsule)
        return CollisionType::CapsuleCapsule;
    if (plane && simplex)
        return CollisionType::PlaneSimplex;
    if (plane)
        return CollisionType::PlanePlane;
    return CollisionType::SimplexSimplex;
}

void findPairs(Query<OptRead<BoxCollider>, OptRead<CapsuleCollider>, OptRead<PlaneCollider>, OptRead<SimplexCollider>,
                     Read<ColliderAABB>>
                   query,
               Write<BroadPhaseCollisions> collisions)
{
    collisions->clearCandidates();
    
    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        for (auto& [entity, overlaps] : collisions->sweepOverlapMaps[axis])
        {
            auto [box, capsule, plane, simplex, aabb] = query[entity].value();
            for (auto& other : overlaps)
            {
                auto [otherBox, otherCapsule, otherPlane, otherSimplex, otherAabb] = query[other].value();

                // TODO: Should this be inside the if statement?
                auto type = getCollisionType(box || otherBox, capsule || otherCapsule, plane || otherPlane,
                                             simplex || otherSimplex);

                switch (axis)
                {
                case 0: // X
                    if (aabb->overlapsY(*otherAabb) && aabb->overlapsZ(*otherAabb))
                        collisions->addCandidate(type, {entity, other});
                    break;
                case 1: // Y
                    if (aabb->overlapsX(*otherAabb) && aabb->overlapsZ(*otherAabb))
                        collisions->addCandidate(type, {entity, other});
                    break;
                case 2: // Z
                    if (aabb->overlapsX(*otherAabb) && aabb->overlapsY(*otherAabb))
                        collisions->addCandidate(type, {entity, other});
                    break;
                }
            }
        }
    }
}