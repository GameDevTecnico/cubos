/// @file
/// @brief Broad phase collision detection systems.

#pragma once

#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/collisions/aabb.hpp>
#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/colliders/box.hpp>
#include <cubos/engine/collisions/colliders/capsule.hpp>
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

/// @brief Adds collision tracking to all new entities with colliders.
template <typename C>
void trackNewEntities(Query<Read<C>, OptRead<ColliderAABB>> query, Write<BroadPhaseCollisions> collisions,
                      Commands commands)
{
    // TODO: This query should eventually be replaced by Query<With<C>, Without<ColliderAABB>>

    for (auto [entity, collider, aabb] : query)
    {
        if (!aabb)
        {
            commands.add(entity, ColliderAABB{});
            collisions->addEntity(entity);
        }
    }
}

/// @brief Updates the AABBs of all box colliders.
void updateBoxAABBs(Query<Read<LocalToWorld>, Read<BoxCollider>, Write<ColliderAABB>> query);

/// @brief Updates the AABBs of all capsule colliders.
void updateCapsuleAABBs(Query<Read<LocalToWorld>, Read<CapsuleCollider>, Write<ColliderAABB>> query,
                        Write<BroadPhaseCollisions> collisions);

/// @brief Updates the sweep markers of all colliders.
void updateMarkers(Query<Read<ColliderAABB>> query, Write<BroadPhaseCollisions> collisions);

/// @brief Performs a sweep of all colliders.
void sweep(Write<BroadPhaseCollisions> collisions);

/// @brief Finds all pairs of colliders which may be colliding.
///
/// @details
/// TODO: This query is disgusting. We need a way to find if a component is present without reading it.
/// Maybe something like Commands but for reads?
void findPairs(Query<OptRead<BoxCollider>, OptRead<CapsuleCollider>, Read<ColliderAABB>> query,
               Write<BroadPhaseCollisions> collisions);