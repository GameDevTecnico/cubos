#include <cubos/engine/collisions/plugin.hpp>

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.addComponent<BoxCollider>();
    cubos.addComponent<SimplexCollider>();
    cubos.addComponent<CapsuleCollider>();
    cubos.addComponent<PlaneCollider>();
}