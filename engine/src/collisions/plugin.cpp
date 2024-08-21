#include "interface/plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "broad_phase/plugin.hpp"
#include "narrow_phase/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::collisionsTag);

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.plugin(interfaceCollisionsPlugin);
    cubos.plugin(broadPhaseCollisionsPlugin);
    cubos.plugin(narrowPhaseCollisionsPlugin);

    cubos.tag(collisionsTag).addTo(collisionsBroadTag).addTo(collisionsNarrowTag).addTo(collisionsManifoldTag);

    auto initializeBoxColliders = [](Query<const BoxCollisionShape&, Collider&> query) {
        for (auto [shape, collider] : query)
        {
            shape.box.diag(collider.localAABB.diag);
            collider.margin = 0.04F;
        }
    };

    cubos.observer("setup Box Colliders").onAdd<BoxCollisionShape>().call(initializeBoxColliders);
    cubos.observer("setup Box Colliders").onAdd<Collider>().call(initializeBoxColliders);

    auto initializeCapsuleColliders = [](Query<const CapsuleCollisionShape&, Collider&> query) {
        for (auto [shape, collider] : query)
        {
            collider.localAABB = shape.capsule.aabb();
            collider.margin = 0.0F;
        }
    };

    cubos.observer("setup Capsule Colliders").onAdd<CapsuleCollisionShape>().call(initializeCapsuleColliders);
    cubos.observer("setup Capsule Colliders").onAdd<Collider>().call(initializeCapsuleColliders);
}
