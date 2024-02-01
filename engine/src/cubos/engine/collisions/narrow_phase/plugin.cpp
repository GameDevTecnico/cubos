#include "plugin.hpp"

#include <cubos/core/geom/intersections.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/transform/local_to_world.hpp>

using namespace cubos::engine;

// detect collisions
static void findCollidingPairsSystem(
    Commands cmds, Query<Entity, const LocalToWorld&, const BoxCollisionShape&, PotentiallyCollidingWith&, Entity,
                         const LocalToWorld&, const BoxCollisionShape&>
                       query)
{
    for (auto [ent1, localToWorld1, boxShape1, potentiallyCollidingWith, ent2, localToWorld2, boxShape2] : query)
    {
        cubos::core::geom::CollisionInfo collision =
            cubos::core::geom::BoxBoxIntersection(boxShape1.box, localToWorld1.mat, boxShape2.box, localToWorld2.mat);

        if (!collision.collided)
        {
            continue;
        }

        cmds.relate(ent1, ent2,
                    CollidingWith{.entity = ent1,
                                  .other = ent2,
                                  .penetration = collision.penetration,
                                  .position = {0.0F, 0.0F, 0.0F},
                                  .normal = collision.normal});
    }
}

/// @brief Removes all pairs of entities found to be potentially colliding.
static void cleanCollidingPairs(Commands cmds, Query<Entity, CollidingWith&, Entity> query)
{
    for (auto [entity, collidingWith, other] : query)
    {
        cmds.unrelate<CollidingWith>(entity, other);
    }
}

void cubos::engine::narrowPhaseCollisionsPlugin(Cubos& cubos)
{
    cubos.addRelation<CollidingWith>();

    cubos.system(cleanCollidingPairs).tagged("cubos.collisions.narrow.clean").before("cubos.collisions.narrow");
    cubos.system(findCollidingPairsSystem).tagged("cubos.collisions.narrow").after("cubos.collisions.broad");
}
