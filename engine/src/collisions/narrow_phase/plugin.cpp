#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/core/geom/intersections.hpp>
#include <cubos/core/geom/plane.hpp>
#include <cubos/core/geom/utils.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../broad_phase/plugin.hpp"
#include "../broad_phase/potentially_colliding_with.hpp"
#include "../interface/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::collisionsNarrowCleanTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsNarrowTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsManifoldTag);

void cubos::engine::narrowPhaseCollisionsPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(interfaceCollisionsPlugin);
    cubos.depends(broadPhaseCollisionsPlugin);

    cubos.tag(collisionsNarrowCleanTag).tagged(fixedStepTag);
    cubos.tag(collisionsNarrowTag).tagged(fixedStepTag);
    cubos.tag(collisionsManifoldTag).tagged(fixedStepTag);

    cubos.system("clean colliding pairs")
        .tagged(collisionsNarrowCleanTag)
        .before(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, CollidingWith&, Entity> query) {
            for (auto [entity, collidingWith, other] : query)
            {
                cmds.unrelate<CollidingWith>(entity, other);
            }
        });

    cubos.system("clean manifolds")
        .tagged(collisionsNarrowCleanTag)
        .before(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, ContactManifold&, Entity> query) {
            for (auto [entity, collidingWith, other] : query)
            {
                cmds.unrelate<ContactManifold>(entity, other);
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

    /// TODO: this can potentially simplified, since we know we're dealing with boxes, the number of points
    // contained in each polygon and the adjacent faces might be always the same.
    cubos.system("collision manifolds")
        .tagged(collisionsManifoldTag)
        .after(collisionsNarrowTag)
        .call([](Commands cmds,
                 Query<Entity, const Position&, const LocalToWorld&, const BoxCollisionShape&, CollidingWith&, Entity,
                       const Position&, const LocalToWorld&, const BoxCollisionShape&>
                     query) {
            for (auto [ent1, position1, localToWorld1, boxShape1, collidingWith, ent2, position2, localToWorld2,
                       boxShape2] : query)
            {
                // If penetration not bigger than 0 continue
                if (collidingWith.penetration < 0)
                {
                    continue;
                }

                // Calculate incident and reference face
                std::list<glm::vec3> polygon1, polygon2;
                glm::vec3 normal1, normal2;
                std::vector<cubos::core::geom::Plane> adjPlanes1, adjPlanes2;
                getIncidentReferencePolygon(boxShape1.box, collidingWith.normal, polygon1, normal1, adjPlanes1,
                                            localToWorld1.mat, localToWorld1.worldScale());
                getIncidentReferencePolygon(boxShape2.box, -collidingWith.normal, polygon2, normal2, adjPlanes2,
                                            localToWorld2.mat, localToWorld2.worldScale());

                // Each face will always have more than 1 point so we proceed to clipping
                // See which one of the normals is the reference one by checking which has the highest dot product
                bool flipped =
                    fabs(glm::dot(collidingWith.normal, normal1)) < fabs(glm::dot(collidingWith.normal, normal2));

                if (flipped)
                {
                    std::swap(polygon1, polygon2);
                    std::swap(normal1, normal2);
                    std::swap(adjPlanes1, adjPlanes2);
                }

                // Clip the incident face to the adjacent edges of the reference face
                if (adjPlanes1.size() > 0)
                {
                    polygon2 = cubos::core::geom::sutherlandHodgmanClipping(polygon2, (int)adjPlanes1.size(),
                                                                            &adjPlanes1[0], false);
                }

                // Finally clip (and remove) any contact points that are above the reference face
                cubos::core::geom::Plane refPlane =
                    cubos::core::geom::Plane{.normal = -normal1, .d = -glm::dot(-normal1, polygon1.front())};
                polygon2 = cubos::core::geom::sutherlandHodgmanClipping(polygon2, 1, &refPlane, true);

                // Use the remaining contact point on the manifold
                std::vector<ContactPointData> points = std::vector<ContactPointData>(polygon2.size());

                for (const glm::vec3& point : polygon2)
                {
                    // Compute distance to reference plane
                    glm::vec3 pointDiff = point - cubos::core::geom::getClosestPointPolygon(point, polygon1);
                    float contactPenetration = -glm::dot(pointDiff, collidingWith.normal); // is this positive

                    // Set Contact data
                    /// TODO: we need to pass these to local space for solving (probably)
                    glm::vec3 globalOnA = point;                                             // world coordinates
                    glm::vec3 globalOnB = point + collidingWith.normal * contactPenetration; // world coordinates

                    // If we flipped incident and reference planes, we will
                    // need to flip it back before sending it to the manifold.
                    if (flipped)
                    {
                        contactPenetration = -contactPenetration;
                        globalOnA = point - collidingWith.normal * contactPenetration;
                        globalOnB = point;
                    }

                    // Just make a final sanity check that the contact point
                    // is actual a point of contact not just a clipping bug
                    // and consider only points with positive penetration
                    if (contactPenetration >= 0.0F)
                    {
                        ContactPointData contact = ContactPointData{.entity = ent1,
                                                                    .position1 = globalOnA,
                                                                    .position2 = globalOnB,
                                                                    .penetration = collidingWith.penetration,
                                                                    .id = 0};

                        points.push_back(contact);
                    }
                }

                cmds.relate(ent1, ent2,
                            ContactManifold{.entity = ent1, .normal = collidingWith.normal, .points = points});
            }
        });
}
