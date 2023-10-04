#include "broad_phase.hpp"

using CollisionType = BroadPhaseCollisions::CollisionType;

void setupNewBoxes(Query<Read<BoxCollisionShape>, Write<Collider>> query, Write<BroadPhaseCollisions> collisions)
{
    for (auto [entity, shape, collider] : query)
    {
        if (collider->fresh)
        {
            collisions->addEntity(entity);

            shape->box.diag(collider->localAABB.diag);

            collider->margin = 0.04F;

            collider->fresh = false;
        }
    }
}

void setupNewCapsules(Query<Read<CapsuleCollisionShape>, Write<Collider>> query, Write<BroadPhaseCollisions> collisions)
{
    (void)query;
    (void)collisions;
}

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

void updateMarkers(Query<Read<Collider>> query, Write<BroadPhaseCollisions> collisions)
{
    // TODO: This is parallelizable.
    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        // TODO: Should use insert sort to leverage spatial coherence.
        std::sort(
            collisions->markersPerAxis[axis].begin(), collisions->markersPerAxis[axis].end(),
            [axis, &query](const BroadPhaseCollisions::SweepMarker& a, const BroadPhaseCollisions::SweepMarker& b) {
                auto [aCollider] = query[a.entity].value();
                auto [bCollider] = query[b.entity].value();
                auto aPos = a.isMin ? aCollider->worldAABB.min() : aCollider->worldAABB.max();
                auto bPos = b.isMin ? bCollider->worldAABB.min() : bCollider->worldAABB.max();
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
                for (const auto& other : collisions->activePerAxis[axis])
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

CollisionType getCollisionType(bool box, bool capsule)
{
    if (box && capsule)
    {
        return CollisionType::BoxCapsule;
    }

    if (box)
    {
        return CollisionType::BoxBox;
    }

    return CollisionType::CapsuleCapsule;
}

void findPairs(Query<OptRead<BoxCollisionShape>, OptRead<CapsuleCollisionShape>, Read<Collider>> query,
               Write<BroadPhaseCollisions> collisions)
{
    collisions->clearCandidates();

    for (glm::length_t axis = 0; axis < 3; axis++)
    {
        for (auto& [entity, overlaps] : collisions->sweepOverlapMaps[axis])
        {
            auto [box, capsule, collider] = query[entity].value();
            for (auto& other : overlaps)
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
                        collisions->addCandidate(type, {entity, other});
                    }
                    break;
                case 1: // Y
                    if (collider->worldAABB.overlapsX(otherCollider->worldAABB) &&
                        collider->worldAABB.overlapsZ(otherCollider->worldAABB))
                    {
                        collisions->addCandidate(type, {entity, other});
                    }
                    break;
                case 2: // Z
                    if (collider->worldAABB.overlapsX(otherCollider->worldAABB) &&
                        collider->worldAABB.overlapsY(otherCollider->worldAABB))
                    {
                        collisions->addCandidate(type, {entity, other});
                    }
                    break;
                }
            }
        }
    }
}