#include "plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>

void cubos::engine::interfaceCollisionsPlugin(Cubos& cubos)
{
    cubos.component<Collider>();
    cubos.component<BoxCollisionShape>();
    cubos.component<CapsuleCollisionShape>();

    cubos.relation<CollidingWith>();
}
