#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/core/geom/intersections.hpp>
#include <cubos/core/geom/plane.hpp>
#include <cubos/core/geom/utils.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/voxel.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../broad_phase/plugin.hpp"
#include "../broad_phase/potentially_colliding_with.hpp"
#include "../interface/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::collisionsNarrowCleanTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsNarrowTag);
CUBOS_DEFINE_TAG(cubos::engine::collisionsManifoldTag);

using cubos::engine::CollidingWith;
using cubos::engine::ContactPointData;
using cubos::engine::LocalToWorld;

std::vector<ContactPointData> computeContactPoints(const cubos::core::geom::Box* matchedShape1,
                                                   const LocalToWorld* matchedLocalToWorld1,
                                                   const cubos::core::geom::Box* matchedShape2,
                                                   const LocalToWorld* matchedLocalToWorld2,
                                                   CollidingWith& collidingWith)
{
    // Calculate incident and reference face
    std::vector<glm::vec3> polygon1;
    std::vector<glm::vec3> polygon2;
    glm::vec3 normal1;
    glm::vec3 normal2;
    std::vector<cubos::core::geom::Plane> adjPlanes1;
    std::vector<cubos::core::geom::Plane> adjPlanes2;
    getIncidentReferencePolygon(*matchedShape1, collidingWith.normal, polygon1, normal1, adjPlanes1,
                                matchedLocalToWorld1->mat, matchedLocalToWorld1->worldScale());
    getIncidentReferencePolygon(*matchedShape2, -collidingWith.normal, polygon2, normal2, adjPlanes2,
                                matchedLocalToWorld2->mat, matchedLocalToWorld2->worldScale());

    // Each face will always have more than 1 point so we proceed to clipping
    // See which one of the normals is the reference one by checking which has the highest dot product
    bool flipped = fabs(glm::dot(collidingWith.normal, normal1)) < fabs(glm::dot(collidingWith.normal, normal2));

    if (flipped)
    {
        std::swap(polygon1, polygon2);
        std::swap(normal1, normal2);
        std::swap(adjPlanes1, adjPlanes2);
    }

    // Clip the incident face to the adjacent edges of the reference face
    polygon2 = cubos::core::geom::sutherlandHodgmanClipping(polygon2, (int)adjPlanes1.size(), adjPlanes1.data(), false);

    // Finally clip (and remove) any contact points that are above the reference face
    cubos::core::geom::Plane refPlane =
        cubos::core::geom::Plane{.normal = -normal1, .d = -glm::dot(-normal1, polygon1.front())};
    polygon2 = cubos::core::geom::sutherlandHodgmanClipping(polygon2, 1, &refPlane, true);

    // Use the remaining contact point on the manifold
    std::vector<ContactPointData> points;

    for (const glm::vec3& point : polygon2)
    {
        // Compute distance to reference plane
        glm::vec3 pointDiff = point - cubos::core::geom::getClosestPointPolygon(point, polygon1);
        float contactPenetration = -glm::dot(pointDiff, collidingWith.normal); // is this positive

        // Set Contact data
        glm::vec3 globalOn1 = point;                                             // world coordinates
        glm::vec3 globalOn2 = point + collidingWith.normal * contactPenetration; // world coordinates

        // If we flipped incident and reference planes, we will
        // need to flip it back before sending it to the manifold.
        if (flipped)
        {
            contactPenetration = -contactPenetration;
            globalOn1 = point - collidingWith.normal * contactPenetration;
            globalOn2 = point;
        }

        glm::vec3 localOn1 = glm::vec3(matchedLocalToWorld1->inverse() * glm::vec4(globalOn1, 1.0F));
        glm::vec3 localOn2 = glm::vec3(matchedLocalToWorld2->inverse() * glm::vec4(globalOn2, 1.0F));

        // Just make a final sanity check that the contact point
        // is actual a point of contact not just a clipping bug
        // and consider only points with positive penetration
        if (contactPenetration >= 0.0F)
        {
            auto contact = ContactPointData{.entity = collidingWith.entity,
                                            .globalOn1 = globalOn1,
                                            .globalOn2 = globalOn2,
                                            .localOn1 = localOn1,
                                            .localOn2 = localOn2,
                                            .penetration = collidingWith.penetration,
                                            .id = 0};

            points.push_back(contact);
        }
    }
    return points;
}

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

    /// Our method to calculate contact manifolds (and all supporting functions) is inspired by the tutorial:
    /// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics5collisionmanifolds/2017%20Tutorial%205%20-%20Collision%20Manifolds.pdf
    /// and the code of the course (Framework 2017):
    /// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/csc8503coderepository/
    /// This system is for box shapes only, so it's simplyfied in comparison to the tutorial.
    cubos.system("collision manifolds")
        .tagged(collisionsManifoldTag)
        .after(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, const LocalToWorld&, const BoxCollisionShape&, CollidingWith&, Entity,
                                      const LocalToWorld&, const BoxCollisionShape&>
                                    query) {
            for (auto [ent1, localToWorld1, boxShape1, collidingWith, ent2, localToWorld2, boxShape2] : query)
            {
                // If penetration not bigger than 0 continue
                if (collidingWith.penetration < 0)
                {
                    continue;
                }

                // Make sure that shape1 corresponds to the entity refered to in collidingWith
                const cubos::core::geom::Box* matchedShape1 = &boxShape1.box;
                const cubos::core::geom::Box* matchedShape2 = &boxShape2.box;
                const LocalToWorld* matchedLocalToWorld1 = &localToWorld1;
                const LocalToWorld* matchedLocalToWorld2 = &localToWorld2;
                if (ent1 != collidingWith.entity)
                {
                    std::swap(matchedShape1, matchedShape2);
                    std::swap(matchedLocalToWorld1, matchedLocalToWorld2);
                }

                auto points = computeContactPoints(matchedShape1, matchedLocalToWorld1, matchedShape2,
                                                   matchedLocalToWorld2, collidingWith);

                cmds.relate(
                    ent1, ent2,
                    ContactManifold{.entity = collidingWith.entity, .normal = collidingWith.normal, .points = points});
            }
        });

    cubos.system("find colliding voxel-box pairs")
        .tagged(collisionsNarrowTag)
        .after(collisionsBroadTag)
        .call(
            [](Commands cmds,
               Query<Entity, const Position&, const LocalToWorld&, const BoxCollisionShape&, PotentiallyCollidingWith&,
                     Entity, const Position&, const LocalToWorld&, const VoxelCollisionShape&>
                   query) {
                for (auto [ent1, position1, localToWorld1, boxShape, potentiallyCollidingWith, ent2, position2,
                           localToWorld2, voxelShape] : query)
                {
                    cubos::core::geom::Intersection intersectionInfo{};
                    for (const auto box : voxelShape.getBoxes())
                    {
                        // Get the current position from the localToWorld matrix
                        glm::mat4 pos2 = localToWorld2.mat; // Store the matrix

                        // Create a translation matrix for the shift
                        glm::mat4 shiftMatrix = glm::translate(glm::mat4(1.0F), -box.shift);

                        pos2 = pos2 * shiftMatrix;

                        bool intersects = cubos::core::geom::intersects(boxShape.box, localToWorld1.mat, box.box, pos2,
                                                                        intersectionInfo);
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
                }
            });

    cubos.system("find colliding voxel-voxel pairs")
        .tagged(collisionsNarrowTag)
        .after(collisionsBroadTag)
        .call([](Commands cmds, Query<Entity, const Position&, const LocalToWorld&, const VoxelCollisionShape&,
                                      PotentiallyCollidingWith&, Entity, const Position&, const LocalToWorld&,
                                      const VoxelCollisionShape&>
                                    query) {
            for (auto [ent1, position1, localToWorld1, voxelShape1, potentiallyCollidingWith, ent2, position2,
                       localToWorld2, voxelShape2] : query)
            {
                cubos::core::geom::Intersection intersectionInfo{};
                intersectionInfo.normal = glm::vec3(0.0F);
                intersectionInfo.penetration = std::numeric_limits<float>::infinity();

                for (const auto box1 : voxelShape1.getBoxes())
                {
                    // Get the current position from the localToWorld matrix
                    glm::mat4 pos1 = localToWorld1.mat; // Store the matrix

                    // Create a translation matrix for the shift
                    glm::mat4 shiftMatrix = glm::translate(glm::mat4(1.0F), -box1.shift);

                    pos1 = pos1 * shiftMatrix;

                    bool intersects = false;
                    for (const auto box2 : voxelShape2.getBoxes())
                    {
                        // Get the current position from the localToWorld matrix
                        glm::mat4 pos2 = localToWorld2.mat; // Store the matrix

                        // Create a translation matrix for the shift
                        glm::mat4 shiftMatrix = glm::translate(glm::mat4(1.0F), -box2.shift);

                        pos2 = pos2 * shiftMatrix;

                        intersects = cubos::core::geom::intersects(box1.box, pos1, box2.box, pos2, intersectionInfo);

                        if (intersects)
                        {
                            cmds.relate(ent1, ent2,
                                        CollidingWith{.entity = ent1,
                                                      .entity1OriginalPosition = position1.vec,
                                                      .entity2OriginalPosition = position2.vec,
                                                      .penetration = intersectionInfo.penetration,
                                                      .position = {0.0F, 0.0F, 0.0F},
                                                      .normal = intersectionInfo.normal});
                        }
                    }
                }
            }
        });

    cubos.system("collision manifolds voxel-box")
        .tagged(collisionsManifoldTag)
        .after(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, const LocalToWorld&, const BoxCollisionShape&, CollidingWith&, Entity,
                                      const LocalToWorld&, const VoxelCollisionShape&>
                                    query) {
            for (auto [ent1, localToWorld1, boxShape, collidingWith, ent2, localToWorld2, voxelShape] : query)
            {
                // If penetration not bigger than 0 continue
                if (collidingWith.penetration < 0)
                {
                    continue;
                }

                std::vector<ContactPointData> points;
                const cubos::core::geom::Box* matchedShape1 = &boxShape.box;
                const LocalToWorld* matchedLocalToWorld1 = &localToWorld1;
                for (auto box : voxelShape.getBoxes())
                {
                    const cubos::core::geom::Box* matchedShape2 = &box.box;
                    LocalToWorld matchedLocalToWorld2 = localToWorld2;

                    // Create a translation matrix for the shift
                    glm::mat4 shiftMatrix = glm::translate(glm::mat4(1.0F), -box.shift);
                    matchedLocalToWorld2.mat *= shiftMatrix;

                    auto newPoints = computeContactPoints(matchedShape1, matchedLocalToWorld1, matchedShape2,
                                                          &matchedLocalToWorld2, collidingWith);
                    points.insert(points.end(), newPoints.begin(), newPoints.end());
                }

                cmds.relate(
                    ent1, ent2,
                    ContactManifold{.entity = collidingWith.entity, .normal = collidingWith.normal, .points = points});
            }
        });

    cubos.system("collision manifolds voxel-voxel")
        .tagged(collisionsManifoldTag)
        .after(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, const LocalToWorld&, const VoxelCollisionShape&, CollidingWith&, Entity,
                                      const LocalToWorld&, const VoxelCollisionShape&>
                                    query) {
            for (auto [ent1, localToWorld1, voxelShape1, collidingWith, ent2, localToWorld2, voxelShape2] : query)
            {
                // If penetration not bigger than 0 continue
                if (collidingWith.penetration < 0)
                {
                    continue;
                }

                std::vector<ContactPointData> points;
                // Make sure that shape1 corresponds to the entity refered to in collidingWith
                if (ent1 == collidingWith.entity)
                {
                    for (auto box1 : voxelShape1.getBoxes())
                    {
                        const cubos::core::geom::Box* matchedShape1 = &box1.box;
                        LocalToWorld matchedLocalToWorld1 = localToWorld1;
                        // Create a translation matrix for the shift
                        glm::mat4 shiftMatrix1 = glm::translate(glm::mat4(1.0F), -box1.shift);
                        matchedLocalToWorld1.mat *= shiftMatrix1;

                        for (auto box2 : voxelShape2.getBoxes())
                        {
                            const cubos::core::geom::Box* matchedShape2 = &box2.box;
                            LocalToWorld matchedLocalToWorld2 = localToWorld2;

                            // Create a translation matrix for the shift
                            glm::mat4 shiftMatrix2 = glm::translate(glm::mat4(1.0F), -box2.shift);
                            matchedLocalToWorld2.mat *= shiftMatrix2;

                            auto newPoints = computeContactPoints(matchedShape1, &matchedLocalToWorld1, matchedShape2,
                                                                  &matchedLocalToWorld2, collidingWith);
                            points.insert(points.end(), newPoints.begin(), newPoints.end());
                        }
                    }
                }
                else
                {
                    for (auto box1 : voxelShape2.getBoxes())
                    {
                        const cubos::core::geom::Box* matchedShape1 = &box1.box;
                        LocalToWorld matchedLocalToWorld1 = localToWorld1;
                        // Create a translation matrix for the shift
                        glm::mat4 shiftMatrix1 = glm::translate(glm::mat4(1.0F), -box1.shift);
                        matchedLocalToWorld1.mat *= shiftMatrix1;

                        for (auto box2 : voxelShape1.getBoxes())
                        {
                            const cubos::core::geom::Box* matchedShape2 = &box2.box;
                            LocalToWorld matchedLocalToWorld2 = localToWorld2;

                            // Create a translation matrix for the shift
                            glm::mat4 shiftMatrix2 = glm::translate(glm::mat4(1.0F), -box2.shift);
                            matchedLocalToWorld2.mat *= shiftMatrix2;

                            auto newPoints = computeContactPoints(matchedShape1, &matchedLocalToWorld1, matchedShape2,
                                                                  &matchedLocalToWorld2, collidingWith);
                            points.insert(points.end(), newPoints.begin(), newPoints.end());
                        }
                    }
                }

                cmds.relate(
                    ent1, ent2,
                    ContactManifold{.entity = collidingWith.entity, .normal = collidingWith.normal, .points = points});
            }
        });
}
