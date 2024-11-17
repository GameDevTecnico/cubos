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

using cubos::core::geom::PolygonalFeature;
using cubos::engine::CollidingWith;
using cubos::engine::ContactFeatureId;
using cubos::engine::ContactPointData;
using cubos::engine::LocalToWorld;

std::vector<ContactPointData> computeContactPoints(const cubos::core::geom::Box* matchedShape1,
                                                   const LocalToWorld* matchedLocalToWorld1,
                                                   const cubos::core::geom::Box* matchedShape2,
                                                   const LocalToWorld* matchedLocalToWorld2,
                                                   CollidingWith& collidingWith)
{
    // Calculate incident and reference face
    PolygonalFeature face1;
    PolygonalFeature face2;
    std::vector<uint32_t> adjPlanes1Ids;
    std::vector<uint32_t> adjPlanes2Ids;
    std::vector<cubos::core::geom::Plane> adjPlanes1;
    std::vector<cubos::core::geom::Plane> adjPlanes2;
    getIncidentReferencePolygon(*matchedShape1, collidingWith.normal, face1, adjPlanes1, adjPlanes1Ids,
                                matchedLocalToWorld1->mat, matchedLocalToWorld1->worldScale());
    getIncidentReferencePolygon(*matchedShape2, -collidingWith.normal, face2, adjPlanes2, adjPlanes2Ids,
                                matchedLocalToWorld2->mat, matchedLocalToWorld2->worldScale());

    // Each face will always have more than 1 point so we proceed to clipping
    // See which one of the normals is the reference one by checking which has the highest dot product
    bool flipped =
        fabs(glm::dot(collidingWith.normal, face1.normal)) < fabs(glm::dot(collidingWith.normal, face2.normal));

    if (flipped)
    {
        std::swap(face1, face2);
        std::swap(adjPlanes1, adjPlanes2);
        std::swap(adjPlanes1Ids, adjPlanes2Ids);
    }

    // Clip the incident face to the adjacent edges of the reference face
    cubos::core::geom::sutherlandHodgmanClipping(face2, (int)adjPlanes1.size(), adjPlanes1.data(), false);

    // Finally clip (and remove) any contact points that are above the reference face
    cubos::core::geom::Plane refPlane =
        cubos::core::geom::Plane{.normal = -face1.normal, .d = -glm::dot(-face1.normal, face1.vertices.front())};
    cubos::core::geom::sutherlandHodgmanClipping(face2, 1, &refPlane, true);

    // Use the remaining contact point on the manifold
    std::vector<ContactPointData> points;

    for (size_t i = 0; i < face2.vertices.size(); i++)
    {
        // Compute distance to reference plane
        glm::vec3 pointDiff =
            face2.vertices[i] - cubos::core::geom::getClosestPointPolygon(face2.vertices[i], face1.vertices);
        float contactPenetration = -glm::dot(pointDiff, collidingWith.normal); // is this positive

        // Set Contact data
        glm::vec3 globalOn1 = face2.vertices[i] + collidingWith.normal * contactPenetration; // world coordinates
        glm::vec3 globalOn2 = face2.vertices[i];                                             // world coordinates

        ContactFeatureId featureId1;
        featureId1.setAsFace(face1.faceId);
        ContactFeatureId featureId2;
        featureId2.setAsVertex(face2.vertexIds[i]);

        // If we flipped incident and reference planes, we will
        // need to flip it back before sending it to the manifold.
        if (flipped)
        {
            contactPenetration = -contactPenetration;
            std::swap(globalOn1, globalOn2);
            std::swap(featureId1, featureId2);
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
                                            .normalImpulse = 0.0F,
                                            .frictionImpulse1 = 0.0F,
                                            .frictionImpulse2 = 0.0F,
                                            .fid1 = featureId1,
                                            .fid2 = featureId2};

            points.push_back(contact);
        }
    }
    return points;
}

static void matchContactPoints(std::vector<ContactPointData>& newPoints, std::vector<ContactPointData>& oldPoints)
{
    for (auto newPoint : newPoints)
    {
        for (auto oldPoint : oldPoints)
        {
            if ((newPoint.fid1 == oldPoint.fid1) && (newPoint.fid2 == oldPoint.fid2))
            {
                newPoint.normalImpulse = oldPoint.normalImpulse;
                newPoint.frictionImpulse1 = oldPoint.frictionImpulse1;
                newPoint.frictionImpulse2 = oldPoint.frictionImpulse2;
                break;
            }
        }
    }
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

    // Remove CollidingWith when it isn't related by PotentiallyCollidingWith
    cubos.system("clean colliding pairs")
        .tagged(collisionsNarrowCleanTag)
        .after(collisionsNarrowTag)
        .before(collisionsManifoldTag)
        .call([](Commands cmds, Query<Entity, PotentiallyCollidingWith&, Entity> pQuery,
                 Query<Entity, CollidingWith&, Entity> cQuery) {
            for (auto [entity, collidingWith, other] : cQuery)
            {
                if (!pQuery.pin(0, entity).pin(1, other).first())
                {
                    cmds.unrelate<CollidingWith>(entity, other);
                    cmds.unrelate<ContactManifold>(entity, other);
                }
            }
        });

    cubos.system("find colliding pairs")
        .tagged(collisionsNarrowTag)
        .after(collisionsBroadTag)
        .call([](Commands cmds,
                 Query<Entity, const Position&, const LocalToWorld&, const BoxCollisionShape&,
                       const PotentiallyCollidingWith&, Entity, const Position&, const LocalToWorld&,
                       const BoxCollisionShape&>
                     nQuery,
                 Query<Entity, const Position&, const LocalToWorld&, const BoxCollisionShape&, CollidingWith&, Entity,
                       const Position&, const LocalToWorld&, const BoxCollisionShape&>
                     yQuery) {
            for (auto [ent1, position1, localToWorld1, boxShape1, potentiallyCollidingWith, ent2, position2,
                       localToWorld2, boxShape2] : nQuery)
            {

                cubos::core::geom::Intersection intersectionInfo{};

                bool intersects = cubos::core::geom::intersects(boxShape1.box, localToWorld1.mat, boxShape2.box,
                                                                localToWorld2.mat, intersectionInfo);

                // If CollidingWith present in previous frame update it
                if (auto match = yQuery.pin(0, ent1).pin(1, ent2).first())
                {
                    if (!intersects)
                    {
                        // Remove CollidingWith when it is related by PotentiallyCollidingWith but not intersecting
                        cmds.unrelate<CollidingWith>(ent1, ent2);
                        cmds.unrelate<ContactManifold>(ent1, ent2);
                        continue;
                    }

                    auto [ent1, position1, localToWorld1, boxShape1, collidingWith, ent2, position2, localToWorld2,
                          boxShape2] = *match;

                    /// TODO: this can be simplyfied when we do the refactor
                    if (collidingWith.entity == ent1)
                    {
                        collidingWith.entity1OriginalPosition = position1.vec;
                        collidingWith.entity2OriginalPosition = position2.vec;
                        collidingWith.penetration = intersectionInfo.penetration;
                        collidingWith.position = {0.0F, 0.0F, 0.0F};
                        collidingWith.normal = intersectionInfo.normal;
                    }
                    else
                    {
                        collidingWith.entity1OriginalPosition = position2.vec;
                        collidingWith.entity2OriginalPosition = position1.vec;
                        collidingWith.penetration = intersectionInfo.penetration;
                        collidingWith.position = {0.0F, 0.0F, 0.0F};
                        collidingWith.normal = -intersectionInfo.normal;
                    }
                }
                else
                {
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

    /// Our method to calculate contact manifolds (and all supporting functions) is inspired by the tutorial:
    /// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics5collisionmanifolds/2017%20Tutorial%205%20-%20Collision%20Manifolds.pdf
    /// and the code of the course (Framework 2017):
    /// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/csc8503coderepository/
    /// This system is for box shapes only, so it's simplyfied in comparison to the tutorial.
    cubos.system("collision manifolds")
        .tagged(collisionsManifoldTag)
        .after(collisionsNarrowTag)
        .call([](Commands cmds,
                 Query<Entity, const LocalToWorld&, const BoxCollisionShape&, CollidingWith&, Entity,
                       const LocalToWorld&, const BoxCollisionShape&>
                     cQuery,
                 Query<Entity, const LocalToWorld&, const BoxCollisionShape&, ContactManifold&, Entity,
                       const LocalToWorld&, const BoxCollisionShape&>
                     ymQuery) {
            for (auto [ent1, localToWorld1, boxShape1, collidingWith, ent2, localToWorld2, boxShape2] : cQuery)
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

                // TODO: in the refactor change this to work with the internal vector of manifolds
                // If Manifold present in previous frame update it
                if (auto match = ymQuery.pin(0, ent1).pin(1, ent2).first())
                {
                    auto [ent1, localToWorld1, boxShape1, contactManifold, ent2, localToWorld2, boxShape2] = *match;
                    contactManifold.normal = collidingWith.normal;
                    matchContactPoints(points, contactManifold.points);
                    contactManifold.points = points;
                }
                else
                {
                    cmds.relate(ent1, ent2,
                                ContactManifold{
                                    .entity = collidingWith.entity, .normal = collidingWith.normal, .points = points});
                }
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
