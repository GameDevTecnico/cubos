#include "plugin.hpp"

#include <cubos/core/geom/intersections.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../broad_phase/plugin.hpp"
#include "../broad_phase/potentially_colliding_with.hpp"
#include "../interface/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::collisionsNarrowCleanTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsNarrowTag);

void cubos::engine::narrowPhaseCollisionsPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(interfaceCollisionsPlugin);
    cubos.depends(broadPhaseCollisionsPlugin);

    cubos.tag(collisionsNarrowCleanTag).tagged(fixedStepTag);
    cubos.tag(collisionsNarrowTag).tagged(fixedStepTag);

    cubos.system("clean colliding pairs")
        .tagged(collisionsNarrowCleanTag)
        .before(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, CollidingWith&, Entity> query) {
            for (auto [entity, collidingWith, other] : query)
            {
                cmds.unrelate<CollidingWith>(entity, other);
            }
        });

    cubos.system("find colliding pairs")
        .tagged(collisionsNarrowTag)
        .after(collisionsBroadTag)
        .call(
            [](Commands cmds,
               Query<Entity, const Position&, const LocalToWorld&, const BoxCollisionShape&, PotentiallyCollidingWith&,
                     Entity, const Position&, const LocalToWorld&, const BoxCollisionShape&>
                   query) {
                for (auto [ent1, position1, localToWorld1, boxShape1, potentiallyCollidingWith, ent2, position2,
                           localToWorld2, boxShape2] : query)
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
                                              .entity1OriginalPosition = position1.vec,
                                              .entity2OriginalPosition = position2.vec,
                                              .penetration = intersectionInfo.penetration,
                                              .position = {0.0F, 0.0F, 0.0F},
                                              .normal = intersectionInfo.normal});
                }
            });
}
