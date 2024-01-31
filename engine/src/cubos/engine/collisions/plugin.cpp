#include "broad_phase/plugin.hpp"

#include <cubos/core/ecs/system/event/writer.hpp>
#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/collision_event.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/transform/plugin.hpp>

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);

    cubos.addPlugin(broadPhaseCollisionsPlugin);

    cubos.addEvent<CollisionEvent>();

    cubos.addComponent<Collider>();
    cubos.addComponent<BoxCollisionShape>();
    cubos.addComponent<CapsuleCollisionShape>();

    cubos.system("setup new boxes")
        .tagged("cubos.collisions.setup")
        .call([](Query<const BoxCollisionShape&, Collider&> query) {
            for (auto [shape, collider] : query)
            {
                if (collider.fresh < 1)
                {
                    collider.fresh++;
                }

                if (collider.fresh == 0)
                {
                    shape.box.diag(collider.localAABB.diag);

                    collider.margin = 0.04F;
                }
            }
        });

    cubos.system("setup new capsules")
        .tagged("cubos.collisions.setup")
        .call([](Query<const CapsuleCollisionShape&, Collider&> query) {
            for (auto [shape, collider] : query)
            {
                if (collider.fresh < 1)
                {
                    collider.fresh++;
                }

                if (collider.fresh == 0)
                {
                    collider.localAABB = shape.capsule.aabb();

                    collider.margin = 0.0F;
                }
            }
        });
}
