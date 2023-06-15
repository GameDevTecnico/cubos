/// @file
/// @brief Broad phase collision detection systems.

#pragma once

#include <cubos/core/ecs/query.hpp>

#include <cubos/engine/collisions/aabb.hpp>
#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/colliders/box.hpp>
#include <cubos/engine/collisions/colliders/capsule.hpp>
#include <cubos/engine/collisions/colliders/plane.hpp>
#include <cubos/engine/collisions/colliders/simplex.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::OptRead;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using cubos::engine::BoxCollider;
using cubos::engine::BroadPhaseCollisions;
using cubos::engine::CapsuleCollider;
using cubos::engine::ColliderAABB;
using cubos::engine::LocalToWorld;
using cubos::engine::PlaneCollider;
using cubos::engine::SimplexCollider;

/// @brief Adds missing AABBs to all colliders.
template <typename C>
void addMissingAABBs(Query<Read<C>, OptRead<ColliderAABB>> query, Commands commands)
{
    // TODO: This query should eventually be replaced by Query<With<C>, Without<ColliderAABB>>

    for (auto [entity, collider, aabb] : query)
    {
        if (!aabb)
        {
            commands.add(entity, ColliderAABB{});
        }
    }
}

/// @brief Updates the AABBs of all box colliders.
void updateBoxAABBs(Query<Read<LocalToWorld>, Read<BoxCollider>, Write<ColliderAABB>> query);

/// @brief Updates the AABBs of all capsule colliders.
void updateCapsuleAABBs(Query<Read<LocalToWorld>, Read<CapsuleCollider>, Write<ColliderAABB>> query,
                        Write<BroadPhaseCollisions> collisions);

/// @brief Updates the AABBs of all simplex colliders.
void updateSimplexAABBs(Query<Read<LocalToWorld>, Read<SimplexCollider>, Write<ColliderAABB>> query,
                        Write<BroadPhaseCollisions> collisions);

/// @brief Updates the sweep markers of all colliders.
void updateMarkers(Query<Read<ColliderAABB>> query, Write<BroadPhaseCollisions> collisions);

/// @brief Performs a sweep of all colliders.
void sweep(Write<BroadPhaseCollisions> collisions);

/// @brief Finds all pairs of colliders which may be colliding.
void findPairs(Write<BroadPhaseCollisions> collisions);