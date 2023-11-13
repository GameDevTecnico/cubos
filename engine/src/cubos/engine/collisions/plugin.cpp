#include "broad_phase/plugin.hpp"

#include <cubos/core/ecs/system/event/writer.hpp>
#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/collisions/broad_phase/candidates.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/collision_event.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

/// @brief Setups new box colliders.
static void setupNewBoxesSystem(Query<const BoxCollisionShape&, Collider&> query)
{
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
}

/// @brief Setups new capsule colliders.
static void setupNewCapsulesSystem(Query<const CapsuleCollisionShape&, Collider&> query)
{
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
}

/// TODO: This is a temporary system should be removed once narrow phase is implemented.
static void emitCollisionEventSystem(Query<const Collider&> query, const BroadPhaseCandidates& candidates,
                                     EventWriter<CollisionEvent> events)
{
    for (const auto& [ent1, ent2] : candidates.candidates(BroadPhaseCandidates::CollisionType::BoxBox))
    {
        auto [collider1] = *query.at(ent1);
        auto [collider2] = *query.at(ent2);

        auto c1 = collider1.worldAABB.center();
        auto b1 = collider1.worldAABB.box();

        auto c2 = collider2.worldAABB.center();
        auto b2 = collider2.worldAABB.box();

        auto d = c2 - c1;
        auto p = (b2.halfSize + b1.halfSize) - glm::abs(d);

        auto s = glm::sign(d);

        float penetration = 0.0F;
        glm::vec3 normal{0.0F};
        glm::vec3 pos{0.0F};

        if (p.x < p.y && p.x < p.z)
        {
            penetration = p.x;
            normal.x = s.x;
            pos.x = c1.x + (b1.halfSize.x * s.x);
        }
        else if (p.y < p.x && p.y < p.z)
        {
            penetration = p.y;
            normal.y = s.y;
            pos.y = c1.y + (b1.halfSize.y * s.y);
        }
        else
        {
            penetration = p.z;
            normal.z = s.z;
            pos.z = c1.z + (b1.halfSize.z * s.z);
        }

        events.push(CollisionEvent{
            .entity = ent1,
            .other = ent2,
            .penetration = penetration,
            .position = pos,
            .normal = normal,
        });
    }
}

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);

    cubos.addPlugin(broadPhaseCollisionsPlugin);

    cubos.addEvent<CollisionEvent>();

    cubos.addComponent<Collider>();
    cubos.addComponent<BoxCollisionShape>();
    cubos.addComponent<CapsuleCollisionShape>();

    cubos.system(setupNewBoxesSystem).tagged("cubos.collisions.setup");
    cubos.system(setupNewCapsulesSystem).tagged("cubos.collisions.setup");
    cubos.system(emitCollisionEventSystem).tagged("cubos.collisions").after("cubos.collisions.setup");
}
