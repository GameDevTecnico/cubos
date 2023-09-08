#include <cubos/engine/collisions/aabb.hpp>
#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/plugin.hpp>

#include "broad_phase.hpp"

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);

    cubos.addResource<BroadPhaseCollisions>();

    cubos.addComponent<ColliderAABB>();
    cubos.addComponent<BoxCollider>();
    cubos.addComponent<SimplexCollider>();
    cubos.addComponent<CapsuleCollider>();
    cubos.addComponent<PlaneCollider>();

    cubos.system(addMissingAABBs<BoxCollider>).tagged("cubos.collisions.aabb.missing");
    cubos.system(addMissingAABBs<SimplexCollider>).tagged("cubos.collisions.aabb.missing");
    cubos.system(addMissingAABBs<CapsuleCollider>).tagged("cubos.collisions.aabb.missing");
    cubos.system(addMissingAABBs<PlaneCollider>).tagged("cubos.collisions.aabb.missing");
    cubos.tag("cubos.collisions.aabb.missing").before("cubos.collisions.aabb");

    cubos.system(updateBoxAABBs).tagged("cubos.collisions.aabb");
    cubos.system(updateCapsuleAABBs).tagged("cubos.collisions.aabb");
    cubos.system(updateSimplexAABBs).tagged("cubos.collisions.aabb");
    cubos.tag("cubos.collisions.aabb").after("cubos.transform.update");

    cubos.system(updateMarkers).tagged("cubos.collisions.broad.markers").after("cubos.collisions.aabb");
    cubos.system(sweep).tagged("cubos.collisions.broad.sweep").after("cubos.collisions.broad.markers");
    cubos.system(findPairs).tagged("cubos.collisions.broad").after("cubos.collisions.broad.sweep");

    cubos.tag("cubos.collisions.broad").before("cubos.collisions");
}
