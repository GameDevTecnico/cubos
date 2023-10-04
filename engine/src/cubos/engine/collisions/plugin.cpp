#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/plugin.hpp>

#include "broad_phase.hpp"

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);

    cubos.addResource<BroadPhaseCollisions>();

    cubos.addComponent<Collider>();
    cubos.addComponent<BoxCollisionShape>();
    cubos.addComponent<CapsuleCollisionShape>();

    cubos.system(trackNewEntities<BoxCollisionShape>).tagged("cubos.collisions.missing");
    cubos.system(trackNewEntities<CapsuleCollisionShape>).tagged("cubos.collisions.missing");
    cubos.tag("cubos.collisions.missing").before("cubos.collisions.aabb.setup");

    cubos.system(setupBoxAABBs).tagged("cubos.collisions.aabb.setup");
    cubos.system(setupCapsuleAABBs).tagged("cubos.collisions.aabb.setup");
    cubos.system(updateAABBs)
        .after("cubos.collisions.aabb.setup")
        .after("cubos.transform.update")
        .before("cubos.collisions.broad.markers");

    cubos.system(updateMarkers).tagged("cubos.collisions.broad.markers");
    cubos.system(sweep).tagged("cubos.collisions.broad.sweep").after("cubos.collisions.broad.markers");
    cubos.system(findPairs).tagged("cubos.collisions.broad").after("cubos.collisions.broad.sweep");
}
