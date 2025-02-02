#include "plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/collisions/shapes/voxel.hpp>

void cubos::engine::interfaceCollisionsPlugin(Cubos& cubos)
{
    cubos.component<Collider>();
    cubos.component<BoxCollisionShape>();
    cubos.component<CapsuleCollisionShape>();
    cubos.component<VoxelCollisionShape>();

    cubos.relation<CollidingWith>();
}
