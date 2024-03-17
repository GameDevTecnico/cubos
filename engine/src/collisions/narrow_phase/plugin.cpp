#include "plugin.hpp"

#include <cubos/core/geom/intersections.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/transform/local_to_world.hpp>

#include "../broad_phase/plugin.hpp"
#include "../broad_phase/potentially_colliding_with.hpp"

CUBOS_DEFINE_TAG(cubos::engine::CollisionsNarrowCleanTag);
CUBOS_DEFINE_TAG(cubos::engine::CollisionsNarrowTag);

void cubos::engine::narrowPhaseCollisionsPlugin(Cubos& cubos)
{
    cubos.addRelation<CollidingWith>();

    cubos.system("clean colliding pairs")
        .tagged(CollisionsNarrowCleanTag)
        .before(CollisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, CollidingWith&, Entity> query) {
            for (auto [entity, collidingWith, other] : query)
            {
                cmds.unrelate<CollidingWith>(entity, other);
            }
        });

    cubos.system("find colliding pairs")
        .tagged(CollisionsNarrowTag)
        .after(CollisionsBroadTag)
        .call([](Commands cmds, Query<Entity, const LocalToWorld&, const BoxCollisionShape&, PotentiallyCollidingWith&,
                                      Entity, const LocalToWorld&, const BoxCollisionShape&>
                                    query) {
            for (auto [ent1, localToWorld1, boxShape1, potentiallyCollidingWith, ent2, localToWorld2, boxShape2] :
                 query)
            {
                cubos::core::geom::Intersection intersectionInfo{};

                bool intersects = cubos::core::geom::intersects(boxShape1.box, localToWorld1.mat, boxShape2.box,
                                                                localToWorld2.mat, intersectionInfo);

                if (!intersects)
                {
                    continue;
                }

                cmds.relate(ent1, ent2,
                            CollidingWith{.entity = ent1,
                                          .penetration = intersectionInfo.penetration,
                                          .position = {0.0F, 0.0F, 0.0F},
                                          .normal = intersectionInfo.normal});
            }
        });
}
