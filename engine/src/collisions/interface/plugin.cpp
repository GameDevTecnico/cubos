#include "plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/collider_aabb.hpp>
#include <cubos/engine/collisions/collider_bundle.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/collision_layers.hpp>
#include <cubos/engine/collisions/collision_mask.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/intersection_end.hpp>
#include <cubos/engine/collisions/intersection_start.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/collisions/shapes/voxel.hpp>

void cubos::engine::interfaceCollisionsPlugin(Cubos& cubos)
{
    cubos.component<BoxCollisionShape>();
    cubos.component<CapsuleCollisionShape>();
    cubos.component<VoxelCollisionShape>();
    cubos.component<CollisionLayers>();
    cubos.component<CollisionMask>();
    cubos.component<ColliderAABB>();
    cubos.component<Collider>();

    // Bundles
    cubos.component<ColliderBundle>();

    cubos.relation<CollidingWith>();
    cubos.relation<IntersectionStart>();
    cubos.relation<IntersectionEnd>();
}
