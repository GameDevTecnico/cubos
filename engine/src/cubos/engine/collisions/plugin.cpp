#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/collisions/broad_phase/plugin.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

/// @brief Setups new box colliders.
void setupNewBoxes(Query<Read<BoxCollisionShape>, Write<Collider>> query)
{
    for (auto [entity, shape, collider] : query)
    {
        if (collider->fresh < 1)
        {
            collider->fresh++;
        }

        if (collider->fresh == 0)
        {
            shape->box.diag(collider->localAABB.diag);

            collider->margin = 0.04F;
        }
    }
}

/// @brief Setups new capsule colliders.
void setupNewCapsules(Query<Read<CapsuleCollisionShape>, Write<Collider>> query)
{
    for (auto [entity, shape, collider] : query)
    {
        if (collider->fresh < 1)
        {
            collider->fresh++;
        }

        if (collider->fresh == 0)
        {
            collider->localAABB = shape->capsule.aabb();

            collider->margin = 0.0F;
        }
    }
}

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);

    cubos.addPlugin(broadPhaseCollisionsPlugin);

    cubos.addComponent<Collider>();
    cubos.addComponent<BoxCollisionShape>();
    cubos.addComponent<CapsuleCollisionShape>();

    cubos.system(setupNewBoxes).tagged("cubos.collisions.setup");
    cubos.system(setupNewCapsules).tagged("cubos.collisions.setup");
}
