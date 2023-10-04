/// @file
/// @brief Broad phase collision detection systems.

#pragma once

#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::OptRead;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using cubos::engine::BoxCollisionShape;
using cubos::engine::BroadPhaseCollisions;
using cubos::engine::CapsuleCollisionShape;
using cubos::engine::Collider;
using cubos::engine::LocalToWorld;

/// @brief Setups new box colliders.
void setupNewBoxes(Query<Read<BoxCollisionShape>, Write<Collider>> query, Write<BroadPhaseCollisions> collisions);

/// @brief Setups new capsule colliders.
void setupNewCapsules(Query<Read<CapsuleCollisionShape>, Write<Collider>> query,
                      Write<BroadPhaseCollisions> collisions);

/// @brief Updates the AABBs of all colliders.
void updateAABBs(Query<Read<LocalToWorld>, Write<Collider>> query);

/// @brief Updates the sweep markers of all colliders.
void updateMarkers(Query<Read<Collider>> query, Write<BroadPhaseCollisions> collisions);

/// @brief Performs a sweep of all colliders.
void sweep(Write<BroadPhaseCollisions> collisions);

/// @brief Finds all pairs of colliders which may be colliding.
///
/// @details
/// TODO: This query is disgusting. We need a way to find if a component is present without reading it.
/// Maybe something like Commands but for reads?
void findPairs(Query<OptRead<BoxCollisionShape>, OptRead<CapsuleCollisionShape>, Read<Collider>> query,
               Write<BroadPhaseCollisions> collisions);
