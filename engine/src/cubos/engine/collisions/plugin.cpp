#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/plugin.hpp>

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.addResource<BroadPhaseCollisions>();

    cubos.addComponent<BoxCollider>();
    cubos.addComponent<SimplexCollider>();
    cubos.addComponent<CapsuleCollider>();
    cubos.addComponent<PlaneCollider>();
}
