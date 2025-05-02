#include "plugin.hpp"
#include <algorithm>

#include <glm/glm.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/geom/intersections.hpp>
#include <cubos/core/geom/plane.hpp>
#include <cubos/core/geom/utils.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/intersection_end.hpp>
#include <cubos/engine/collisions/intersection_start.hpp>
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

using cubos::core::ecs::Entity;
using cubos::core::geom::PolygonalFeature;
using cubos::engine::CollidingWith;
using cubos::engine::ContactFeatureId;
using cubos::engine::ContactPointData;
using cubos::engine::LocalToWorld;

std::vector<ContactPointData> computeContactPoints(const cubos::core::geom::Box* matchedShape1,
                                                   const LocalToWorld* matchedLocalToWorld1,
                                                   const glm::mat4* shiftedLocalToWorldMat1,
                                                   const cubos::core::geom::Box* matchedShape2,
                                                   const LocalToWorld* matchedLocalToWorld2,
                                                   const glm::mat4* shiftedLocalToWorldMat2,
                                                   cubos::core::geom::Intersection& intersectionInfo, const Entity ent)
{
    // Calculate incident and reference face
    PolygonalFeature face1;
    PolygonalFeature face2;
    std::vector<uint32_t> adjPlanes1Ids;
    std::vector<uint32_t> adjPlanes2Ids;
    std::vector<cubos::core::geom::Plane> adjPlanes1;
    std::vector<cubos::core::geom::Plane> adjPlanes2;
    getIncidentReferencePolygon(*matchedShape1, intersectionInfo.normal, face1, adjPlanes1, adjPlanes1Ids,
                                *shiftedLocalToWorldMat1, matchedLocalToWorld1->worldScale());
    getIncidentReferencePolygon(*matchedShape2, -intersectionInfo.normal, face2, adjPlanes2, adjPlanes2Ids,
                                *shiftedLocalToWorldMat2, matchedLocalToWorld2->worldScale());

    // Each face will always have more than 1 point so we proceed to clipping
    // See which one of the normals is the reference one by checking which has the highest dot product
    bool flipped =
        fabs(glm::dot(intersectionInfo.normal, face1.normal)) < fabs(glm::dot(intersectionInfo.normal, face2.normal));

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
        float contactPenetration = -glm::dot(pointDiff, intersectionInfo.normal); // is this positive

        // Set Contact data
        glm::vec3 globalOn1 = face2.vertices[i] + intersectionInfo.normal * contactPenetration; // world coordinates
        glm::vec3 globalOn2 = face2.vertices[i];                                                // world coordinates

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
            auto contact = ContactPointData{.entity = ent,
                                            .globalOn1 = globalOn1,
                                            .globalOn2 = globalOn2,
                                            .localOn1 = localOn1,
                                            .localOn2 = localOn2,
                                            .penetration = intersectionInfo.penetration,
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
                    cmds.relate(entity, other, IntersectionEnd{});
                }
            }
        });

    cubos.system("clean onEnteredArea relations")
        .before(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, IntersectionStart&, Entity> query) {
            for (auto [entity, intersectionStart, other] : query)
            {
                cmds.unrelate<IntersectionStart>(entity, other);
            }
        });

    cubos.system("clean onExitedArea relations")
        .before(collisionsNarrowTag)
        .call([](Commands cmds, Query<Entity, IntersectionEnd&, Entity> query) {
            for (auto [entity, intersectionEnd, other] : query)
            {
                cmds.unrelate<IntersectionEnd>(entity, other);
            }
        });

    /// This system is for collisions between physics bodies since they collide and need collision manifolds.
    /// Our method to calculate contact manifolds (and all supporting functions) is inspired by the tutorial:
    /// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics5collisionmanifolds/2017%20Tutorial%205%20-%20Collision%20Manifolds.pdf
    /// and the code of the course (Framework 2017):
    /// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/csc8503coderepository/
    /// This system is for box shapes only, so it's simplyfied in comparison to the tutorial.
    cubos.system("find colliding pairs")
        .tagged(collisionsNarrowTag)
        .after(collisionsBroadTag)
        .call([](Commands cmds,
                 Query<Entity, const LocalToWorld&, const BoxCollisionShape&, const Collider&,
                       const PotentiallyCollidingWith&, Entity, const LocalToWorld&, const BoxCollisionShape&,
                       const Collider&>
                     nQuery,
                 Query<Entity, const LocalToWorld&, const BoxCollisionShape&, CollidingWith&, Entity,
                       const LocalToWorld&, const BoxCollisionShape&>
                     yQuery) {
            for (auto [ent1, localToWorld1, boxShape1, collider1, potentiallyCollidingWith, ent2, localToWorld2,
                       boxShape2, collider2] : nQuery)
            {
                cubos::core::geom::Intersection intersectionInfo{};

                bool intersects = cubos::core::geom::intersects(boxShape1.box, localToWorld1.mat, boxShape2.box,
                                                                localToWorld2.mat, intersectionInfo);

                auto match = yQuery.pin(0, ent1).pin(1, ent2).first();

                // Make sure that shape1 corresponds to the entity refered to in collidingWith
                const cubos::core::geom::Box* matchedShape1 = &boxShape1.box;
                const cubos::core::geom::Box* matchedShape2 = &boxShape2.box;
                const LocalToWorld* matchedLocalToWorld1 = &localToWorld1;
                const LocalToWorld* matchedLocalToWorld2 = &localToWorld2;

                // If CollidingWith present in previous frame update it
                if (match)
                {
                    if (!intersects || (intersects && intersectionInfo.penetration < 0))
                    {
                        // Remove CollidingWith when it is related by PotentiallyCollidingWith but not intersecting
                        cmds.unrelate<CollidingWith>(ent1, ent2);
                        cmds.relate(ent1, ent2, IntersectionEnd{});
                        continue;
                    }

                    // if either is area do not calculate manifolds
                    if (collider1.isArea || collider2.isArea)
                    {
                        continue;
                    }

                    auto [ent1, localToWorld1, boxShape1, collidingWith, ent2, localToWorld2, boxShape2] = *match;

                    if (ent1 != collidingWith.entity)
                    {
                        std::swap(matchedShape1, matchedShape2);
                        std::swap(matchedLocalToWorld1, matchedLocalToWorld2);
                        intersectionInfo.normal *= -1;
                    }

                    auto points = computeContactPoints(matchedShape1, matchedLocalToWorld1, &matchedLocalToWorld1->mat,
                                                       matchedShape2, matchedLocalToWorld2, &matchedLocalToWorld2->mat,
                                                       intersectionInfo, collidingWith.entity);

                    if (!collidingWith.manifolds.empty())
                    {
                        collidingWith.manifolds[0].normal = intersectionInfo.normal;
                        matchContactPoints(points, collidingWith.manifolds[0].points);
                        collidingWith.manifolds[0].points = points;
                    }
                    else
                    {
                        collidingWith.manifolds.push_back(ContactManifold{
                            .normal = intersectionInfo.normal, .points = points, .boxId1 = 1, .boxId2 = 1});
                    }
                }
                else
                {
                    if (!intersects)
                    {
                        continue;
                    }

                    auto collidingWith = CollidingWith{.entity = ent1, .manifolds = {}};

                    // if either is area do not calculate manifolds
                    if (!collider1.isArea && !collider2.isArea)
                    {
                        auto points = computeContactPoints(
                            matchedShape1, matchedLocalToWorld1, &matchedLocalToWorld1->mat, matchedShape2,
                            matchedLocalToWorld2, &matchedLocalToWorld2->mat, intersectionInfo, ent1);

                        collidingWith.manifolds.push_back(ContactManifold{
                            .normal = intersectionInfo.normal, .points = points, .boxId1 = 1, .boxId2 = 1});
                    }

                    cmds.relate(ent1, ent2, collidingWith);
                    cmds.relate(ent1, ent2, IntersectionStart{});
                }
            }
        });

    cubos.system("find colliding voxel-box pairs")
        .tagged(collisionsNarrowTag)
        .after(collisionsBroadTag)
        .call(
            [](Commands cmds,
               Query<Entity, const LocalToWorld&, const BoxCollisionShape&, const Collider&, PotentiallyCollidingWith&,
                     Entity, const LocalToWorld&, const VoxelCollisionShape&, const Collider&>
                   nQuery,
               Query<Entity, const LocalToWorld&, const BoxCollisionShape&, CollidingWith&, Entity, const LocalToWorld&,
                     const VoxelCollisionShape&>
                   yQuery) {
                for (auto [ent1, localToWorld1, boxShape, collider1, potentiallyCollidingWith, ent2, localToWorld2,
                           voxelShape, collider2] : nQuery)
                {
                    cubos::core::geom::Intersection intersectionInfo{};
                    auto match = yQuery.pin(0, ent1).pin(1, ent2).first();
                    std::vector<ContactManifold> newManifolds;
                    bool anyIntersects = false;

                    for (const auto box : voxelShape.getBoxes())
                    {
                        // Get the current position from the localToWorld matrix
                        glm::mat4 shiftedLocalToWorldMat = localToWorld2.mat; // Store the matrix
                        // Create a translation matrix for the shift
                        glm::mat4 shiftMatrix = glm::translate(glm::mat4(1.0F), -box.shift);
                        shiftedLocalToWorldMat = shiftedLocalToWorldMat * shiftMatrix;

                        bool intersects = cubos::core::geom::intersects(boxShape.box, localToWorld1.mat, box.box,
                                                                        shiftedLocalToWorldMat, intersectionInfo);

                        // If penetration not bigger than 0 continue
                        if (intersects)
                        {
                            anyIntersects = true;
                        }

                        // if either is area do not calculate manifolds
                        if (collider1.isArea || collider2.isArea)
                        {
                            continue;
                        }

                        // If CollidingWith present in previous frame update it
                        if (match)
                        {
                            auto [ent1, localToWorld1, boxShape, collidingWith, ent2, localToWorld2, voxelShape] =
                                *match;

                            if (!intersects)
                            {
                                auto it =
                                    std::remove_if(collidingWith.manifolds.begin(), collidingWith.manifolds.end(),
                                                   [&](const auto& manifold) { return manifold.boxId2 == box.boxId; });

                                collidingWith.manifolds.erase(it, collidingWith.manifolds.end());
                                continue;
                            }

                            auto points = computeContactPoints(&boxShape.box, &localToWorld1, &localToWorld1.mat,
                                                               &box.box, &localToWorld2, &shiftedLocalToWorldMat,
                                                               intersectionInfo, collidingWith.entity);

                            bool existed = false;
                            for (auto& manifold : collidingWith.manifolds)
                            {
                                if (manifold.boxId2 == box.boxId)
                                {
                                    manifold.normal = intersectionInfo.normal;
                                    matchContactPoints(points, manifold.points);
                                    manifold.points = points;
                                    existed = true;
                                    break;
                                }
                            }

                            if (!existed)
                            {
                                collidingWith.manifolds.push_back(ContactManifold{.normal = intersectionInfo.normal,
                                                                                  .points = points,
                                                                                  .boxId1 = 1,
                                                                                  .boxId2 = box.boxId});
                            }
                        }
                        else
                        {
                            if (!intersects)
                            {
                                continue;
                            }

                            auto points =
                                computeContactPoints(&boxShape.box, &localToWorld1, &localToWorld1.mat, &box.box,
                                                     &localToWorld2, &shiftedLocalToWorldMat, intersectionInfo, ent1);

                            newManifolds.push_back(ContactManifold{
                                .normal = intersectionInfo.normal, .points = points, .boxId1 = 1, .boxId2 = box.boxId});
                        }
                    }
                    if (!match && anyIntersects)
                    {
                        cmds.relate(ent1, ent2, CollidingWith{.entity = ent1, .manifolds = newManifolds});
                        cmds.relate(ent1, ent2, IntersectionStart{});
                    }
                    else if (match && !anyIntersects)
                    {
                        cmds.unrelate<CollidingWith>(ent1, ent2);
                        cmds.relate(ent1, ent2, IntersectionEnd{});
                    }
                }
            });

    cubos.system("find colliding voxel-voxel pairs")
        .tagged(collisionsNarrowTag)
        .after(collisionsBroadTag)
        .call([](Commands cmds,
                 Query<Entity, const LocalToWorld&, const VoxelCollisionShape&, const Collider&,
                       PotentiallyCollidingWith&, Entity, const LocalToWorld&, const VoxelCollisionShape&,
                       const Collider&>
                     nQuery,
                 Query<Entity, const LocalToWorld&, const VoxelCollisionShape&, CollidingWith&, Entity,
                       const LocalToWorld&, const VoxelCollisionShape&>
                     yQuery) {
            for (auto [ent1, localToWorld1, voxelShape1, collider1, potentiallyCollidingWith, ent2, localToWorld2,
                       voxelShape2, collider2] : nQuery)
            {
                cubos::core::geom::Intersection intersectionInfo{};

                auto match = yQuery.pin(0, ent1).pin(1, ent2).first();

                std::vector<ContactManifold> newManifolds;
                bool anyIntersects = false;

                for (const auto box1 : voxelShape1.getBoxes())
                {
                    // Get the current position from the localToWorld matrix
                    glm::mat4 shiftedLocalToWorldMat1 = localToWorld1.mat; // Store the matrix
                    // Create a translation matrix for the shift
                    glm::mat4 shiftMatrix1 = glm::translate(glm::mat4(1.0F), -box1.shift);
                    shiftedLocalToWorldMat1 = shiftedLocalToWorldMat1 * shiftMatrix1;

                    for (const auto box2 : voxelShape2.getBoxes())
                    {
                        // Get the current position from the localToWorld matrix
                        glm::mat4 shiftedLocalToWorldMat2 = localToWorld2.mat; // Store the matrix
                        // Create a translation matrix for the shift
                        glm::mat4 shiftMatrix2 = glm::translate(glm::mat4(1.0F), -box2.shift);
                        shiftedLocalToWorldMat2 = shiftedLocalToWorldMat2 * shiftMatrix2;

                        bool intersects = cubos::core::geom::intersects(box1.box, shiftedLocalToWorldMat1, box2.box,
                                                                        shiftedLocalToWorldMat2, intersectionInfo);

                        if (intersects)
                        {
                            anyIntersects = true;
                        }

                        // if either is area do not calculate manifolds
                        if (collider1.isArea || collider2.isArea)
                        {
                            continue;
                        }

                        // Make sure that shape1 corresponds to the entity refered to in collidingWith
                        const cubos::core::geom::Box* matchedShape1 = &box1.box;
                        const cubos::core::geom::Box* matchedShape2 = &box2.box;
                        const LocalToWorld* matchedLocalToWorld1 = &localToWorld1;
                        const LocalToWorld* matchedLocalToWorld2 = &localToWorld2;
                        const glm::mat4* matchedShiftedLocalToWorld1 = &shiftedLocalToWorldMat1;
                        const glm::mat4* matchedShiftedLocalToWorld2 = &shiftedLocalToWorldMat2;

                        // If CollidingWith present in previous frame update it
                        if (match)
                        {
                            auto [ent1, localToWorld1, voxelShape1, collidingWith, ent2, localToWorld2, voxelShape2] =
                                *match;

                            if (!intersects || (intersects && intersectionInfo.penetration < 0))
                            {
                                auto it = std::remove_if(collidingWith.manifolds.begin(), collidingWith.manifolds.end(),
                                                         [&](const auto& manifold) {
                                                             return manifold.boxId2 == box2.boxId &&
                                                                    manifold.boxId1 == box1.boxId;
                                                         });
                                collidingWith.manifolds.erase(it, collidingWith.manifolds.end());
                                continue;
                            }

                            if (ent1 != collidingWith.entity)
                            {
                                std::swap(matchedShape1, matchedShape2);
                                std::swap(matchedLocalToWorld1, matchedLocalToWorld2);
                                std::swap(shiftedLocalToWorldMat1, shiftedLocalToWorldMat2);
                                intersectionInfo.normal *= -1;
                            }

                            auto points =
                                computeContactPoints(matchedShape1, matchedLocalToWorld1, matchedShiftedLocalToWorld1,
                                                     matchedShape2, matchedLocalToWorld2, matchedShiftedLocalToWorld2,
                                                     intersectionInfo, collidingWith.entity);

                            bool existed = false;
                            for (auto& manifold : collidingWith.manifolds)
                            {
                                if (manifold.boxId1 == box1.boxId && manifold.boxId2 == box2.boxId)
                                {
                                    manifold.normal = intersectionInfo.normal;
                                    matchContactPoints(points, manifold.points);
                                    manifold.points = points;
                                    existed = true;
                                    break;
                                }
                            }
                            if (!existed)
                            {
                                collidingWith.manifolds.push_back(ContactManifold{.normal = intersectionInfo.normal,
                                                                                  .points = points,
                                                                                  .boxId1 = box1.boxId,
                                                                                  .boxId2 = box2.boxId});
                            }
                        }
                        else
                        {
                            if (!intersects)
                            {
                                continue;
                            }

                            auto points = computeContactPoints(
                                matchedShape1, matchedLocalToWorld1, matchedShiftedLocalToWorld1, matchedShape2,
                                matchedLocalToWorld2, matchedShiftedLocalToWorld2, intersectionInfo, ent1);
                            newManifolds.push_back(ContactManifold{.normal = intersectionInfo.normal,
                                                                   .points = points,
                                                                   .boxId1 = box1.boxId,
                                                                   .boxId2 = box2.boxId});
                        }
                    }
                }
                if (!match && anyIntersects)
                {
                    cmds.relate(ent1, ent2, CollidingWith{.entity = ent1, .manifolds = newManifolds});
                    cmds.relate(ent1, ent2, IntersectionStart{});
                }
                else if (match && !anyIntersects)
                {
                    cmds.unrelate<CollidingWith>(ent1, ent2);
                    cmds.relate(ent1, ent2, IntersectionEnd{});
                }
            }
        });
}
