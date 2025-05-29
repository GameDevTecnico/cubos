#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>
#include <cubos/engine/tools/transform_gizmo/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::World;
using cubos::core::io::Window;

using namespace cubos::engine;

static glm::vec3 intersectLinePlane(glm::vec3 linePoint, glm::vec3 lineVector, glm::vec3 planePoint,
                                    glm::vec3 planeNormal)
{
    float distance;
    if (glm::intersectRayPlane(linePoint, lineVector, planePoint, planeNormal, distance))
    {
        return linePoint + lineVector * distance;
    }
    if (glm::intersectRayPlane(linePoint, -lineVector, planePoint, planeNormal, distance))
    {
        return linePoint - lineVector * distance;
    }
    return {0, 0, 0};
}

static glm::vec3 mouseWorldDirection(glm::ivec2 windowSize, glm::ivec2 mousePos, const glm::mat4& pv)
{
    float mouseX = (float)mousePos.x / ((float)windowSize.x * 0.5F) - 1.0F;
    float mouseY = (float)mousePos.y / ((float)windowSize.y * 0.5F) - 1.0F;

    return glm::normalize(glm::vec3(glm::inverse(pv) * glm::vec4(mouseX, -mouseY, 1.0F, 1.0F)));
}

static glm::vec3 mouseWorldCoordinates(glm::ivec2 windowSize, glm::ivec2 mousePos, glm::vec3 transformPosition,
                                       glm::vec3 transformNormal, glm::vec3 cameraPosition, const glm::mat4& pv)
{
    auto dir = mouseWorldDirection(windowSize, mousePos, pv);
    return intersectLinePlane(cameraPosition, dir, transformPosition, transformNormal);
}

static void checkAxialMovement(Position& position, glm::vec3 globalPosition, glm::vec3 transformVector,
                               glm::vec3 transformNormal, glm::ivec2 windowSize, const Input& input,
                               const glm::mat4& pv, glm::vec3 cameraPosition)
{
    auto mousePosWorld =
        mouseWorldCoordinates(windowSize, input.mousePosition(), globalPosition, transformNormal, cameraPosition, pv);
    auto oldMousePosWorld = mouseWorldCoordinates(windowSize, input.previousMousePosition(), globalPosition,
                                                  transformNormal, cameraPosition, pv);
    auto mousDisp = (mousePosWorld - oldMousePosWorld);
    position.vec +=
        (glm::dot(mousDisp, transformVector) / glm::dot(transformVector, transformVector)) * transformVector;
}

static void checkPlanarMovement(Position& position, glm::vec3 globalPosition, glm::vec3 transformNormal,
                                glm::ivec2 windowSize, const Input& input, const glm::mat4& pv,
                                glm::vec3 cameraPosition)
{
    auto mousePosWorld =
        mouseWorldCoordinates(windowSize, input.mousePosition(), globalPosition, transformNormal, cameraPosition, pv);
    auto oldMousePosWorld = mouseWorldCoordinates(windowSize, input.previousMousePosition(), globalPosition,
                                                  transformNormal, cameraPosition, pv);
    auto mousDisp = (mousePosWorld - oldMousePosWorld);
    position.vec +=
        mousDisp - (glm::dot(mousDisp, transformNormal) / glm::dot(transformNormal, transformNormal)) * transformNormal;
}

static void checkRotation(Rotation& rotation, glm::vec3 globalPosition, glm::vec3 axisVector, glm::ivec2 windowSize,
                          const Input& input, const glm::mat4& pv, glm::vec3 cameraPosition)
{
    auto mousePosWorld =
        mouseWorldCoordinates(windowSize, input.mousePosition(), globalPosition, axisVector, cameraPosition, pv);
    auto oldMousePosWorld = mouseWorldCoordinates(windowSize, input.previousMousePosition(), globalPosition, axisVector,
                                                  cameraPosition, pv);
    auto oldVec = oldMousePosWorld - globalPosition;
    auto newVec = mousePosWorld - globalPosition;

    if (glm::length2(oldVec) == 0 || glm::length2(newVec) == 0)
    {
        return;
    }

    auto angle = glm::orientedAngle(glm::normalize(oldVec), glm::normalize(newVec), axisVector);
    rotation.quat = glm::angleAxis(angle, axisVector) * rotation.quat;
}

static void drawPositionGizmo(Query<Position&, LocalToWorld&> positionQuery, const Camera& camera,
                              const LocalToWorld& cameraLtw, Gizmos& gizmos, const Input& input, const Window& window,
                              Entity selectedEntity, bool useLocalAxis, double distance, Entity cameraEntity)
{
    auto [position, localToWorld] = *positionQuery.at(selectedEntity);

    auto cameraPosition = cameraLtw.worldPosition();

    auto pv = cameraLtw.mat;

    pv = glm::inverse(pv);

    pv = camera.projection * pv;

    glm::vec3 xColor = {1, 0, 0};
    glm::vec3 yColor = {0, 1, 0};
    glm::vec3 zColor = {0, 0, 1};

    glm::vec3 xyColor = {1, 1, 0};
    glm::vec3 xzColor = {1, 0, 1};
    glm::vec3 yzColor = {0, 1, 1};

    glm::vec3 rightVector = {1, 0, 0};
    glm::vec3 upVector = {0, 1, 0};
    glm::vec3 forwardVector = {0, 0, 1};

    if (useLocalAxis)
    {

        rightVector = glm::normalize(localToWorld.worldRotation() * rightVector);
        upVector = glm::normalize(localToWorld.worldRotation() * upVector);
        forwardVector = glm::normalize(localToWorld.worldRotation() * forwardVector);
    }

    // Axis movement
    if (gizmos.locked("transform_gizmo.position.x"))
    {
        checkAxialMovement(position, localToWorld.worldPosition(), rightVector, upVector, window->size(), input, pv,
                           cameraPosition);
        xColor = {1, 1, 0};
    }
    if (gizmos.locked("transform_gizmo.position.y"))
    {
        checkAxialMovement(position, localToWorld.worldPosition(), upVector, rightVector, window->size(), input, pv,
                           cameraPosition);
        yColor = {1, 1, 0};
    }
    if (gizmos.locked("transform_gizmo.position.z"))
    {
        checkAxialMovement(position, localToWorld.worldPosition(), forwardVector, upVector, window->size(), input, pv,
                           cameraPosition);
        zColor = {1, 1, 0};
    }

    // Planar movement
    if (gizmos.locked("transform_gizmo.position.xy"))
    {
        checkPlanarMovement(position, localToWorld.worldPosition(), forwardVector, window->size(), input, pv,
                            cameraPosition);
        xyColor = {1, 1, 1};
    }
    if (gizmos.locked("transform_gizmo.position.xz"))
    {
        checkPlanarMovement(position, localToWorld.worldPosition(), upVector, window->size(), input, pv,
                            cameraPosition);
        xzColor = {1, 1, 1};
    }
    if (gizmos.locked("transform_gizmo.position.yz"))
    {
        checkPlanarMovement(position, localToWorld.worldPosition(), rightVector, window->size(), input, pv,
                            cameraPosition);
        yzColor = {1, 1, 1};
    }

    auto drawPosition =
        cameraPosition + glm::normalize(localToWorld.worldPosition() - cameraPosition) * (float)distance;

    gizmos.color(xyColor);
    glm::mat4 transformXY = glm::translate(glm::mat4(1.0F), drawPosition + (rightVector + upVector) * 0.125F);
    if (useLocalAxis)
    {
        transformXY = transformXY * glm::toMat4(localToWorld.worldRotation());
    }
    transformXY = glm::scale(transformXY, glm::vec3(0.25F, 0.25F, 0.015F));

    gizmos.drawBox("transform_gizmo.position.xy", transformXY, {cameraEntity});
    gizmos.color(xzColor);
    glm::mat4 transformXZ = glm::translate(glm::mat4(1.0F), drawPosition + (rightVector + forwardVector) * 0.125F);
    if (useLocalAxis)
    {
        transformXZ = transformXZ * glm::toMat4(localToWorld.worldRotation());
    }
    transformXZ = glm::scale(transformXZ, glm::vec3(0.25F, 0.015F, 0.25F));

    gizmos.drawBox("transform_gizmo.position.xz", transformXZ, {cameraEntity});
    gizmos.color(yzColor);
    glm::mat4 transformYZ = glm::translate(glm::mat4(1.0F), drawPosition + (upVector + forwardVector) * 0.125F);
    if (useLocalAxis)
    {
        transformYZ = transformYZ * glm::toMat4(localToWorld.worldRotation());
    }
    transformYZ = glm::scale(transformYZ, glm::vec3(0.015F, 0.25F, 0.25F));
    gizmos.drawBox("transform_gizmo.position.yz", transformYZ, {cameraEntity});

    gizmos.color({0.7F, 0.7F, 0.7F});
    gizmos.drawBox("", drawPosition + glm::vec3{0.03F, 0.03F, 0.03F}, drawPosition - glm::vec3{0.03F, 0.03F, 0.03F},
                   {cameraEntity});

    gizmos.color(xColor);
    gizmos.drawArrow("transform_gizmo.position.x", drawPosition + rightVector * 0.03F, rightVector, 0.03F, 0.07F, 0.7F,
                     {cameraEntity});
    gizmos.color(yColor);
    gizmos.drawArrow("transform_gizmo.position.y", drawPosition + upVector * 0.03F, upVector, 0.03F, 0.07F, 0.7F,
                     {cameraEntity});
    gizmos.color(zColor);
    gizmos.drawArrow("transform_gizmo.position.z", drawPosition + forwardVector * 0.03F, forwardVector, 0.03F, 0.07F,
                     0.7F, {cameraEntity});
}

static void drawRotationGizmo(Query<Rotation&, LocalToWorld&> positionQuery, const Camera& camera,
                              const LocalToWorld& cameraLtw, Gizmos& gizmos, const Input& input, const Window& window,
                              Entity selectedEntity, bool useLocalAxis, double distance, Entity cameraEntity)
{
    auto [rotation, localToWorld] = *positionQuery.at(selectedEntity);

    auto cameraPosition = cameraLtw.worldPosition();

    auto pv = cameraLtw.mat;

    pv = glm::inverse(pv);

    pv = camera.projection * pv;

    glm::vec3 xColor = {1, 0, 0};
    glm::vec3 yColor = {0, 1, 0};
    glm::vec3 zColor = {0, 0, 1};

    glm::vec3 rightVector = {1, 0, 0};
    glm::vec3 upVector = {0, 1, 0};
    glm::vec3 forwardVector = {0, 0, 1};

    if (useLocalAxis)
    {
        rightVector = glm::normalize(localToWorld.worldRotation() * rightVector);
        upVector = glm::normalize(localToWorld.worldRotation() * upVector);
        forwardVector = glm::normalize(localToWorld.worldRotation() * forwardVector);
    }

    if (gizmos.locked("transform_gizmo.rotation.x"))
    {
        checkRotation(rotation, localToWorld.worldPosition(), rightVector, window->size(), input, pv, cameraPosition);
        xColor = {1, 1, 0};
    }
    if (gizmos.locked("transform_gizmo.rotation.y"))
    {
        checkRotation(rotation, localToWorld.worldPosition(), upVector, window->size(), input, pv, cameraPosition);
        yColor = {1, 1, 0};
    }
    if (gizmos.locked("transform_gizmo.rotation.z"))
    {
        checkRotation(rotation, localToWorld.worldPosition(), forwardVector, window->size(), input, pv, cameraPosition);
        zColor = {1, 1, 0};
    }

    auto drawPosition =
        cameraPosition + glm::normalize(localToWorld.worldPosition() - cameraPosition) * (float)distance;

    gizmos.color(xColor);
    gizmos.drawRing("transform_gizmo.rotation.x", drawPosition + (rightVector * 0.1F),
                    drawPosition - (rightVector * 0.01F), 1.4F, 1.25F, {cameraEntity});
    gizmos.color(yColor);
    gizmos.drawRing("transform_gizmo.rotation.y", drawPosition + (upVector * 0.1F), drawPosition - (upVector * 0.01F),
                    1.4F, 1.25F, {cameraEntity});
    gizmos.color(zColor);
    gizmos.drawRing("transform_gizmo.rotation.z", drawPosition + (forwardVector * 0.1F),
                    drawPosition - (forwardVector * 0.01F), 1.4F, 1.25F, {cameraEntity});
}

void cubos::engine::transformGizmoPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(gizmosPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(settingsPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(windowPlugin);

    cubos.depends(selectionPlugin);

    cubos.system("draw Transform Gizmo")
        .after(transformUpdateTag)
        .call([](const Selection& selection, const Window& window, const Input& input, Settings& settings,
                 Gizmos& gizmos, Query<Position&, LocalToWorld&> positionQuery,
                 Query<Rotation&, LocalToWorld&> rotationQuery,
                 Query<Entity, const Camera&, LocalToWorld&, const DrawsTo&, const GizmosTarget&> cameraQuery) {
            if (selection.entity.isNull())
            {
                return;
            }

            if (!positionQuery.at(selection.entity))
            {
                return;
            }

            bool useLocal = settings.getBool("transformGizmo.useLocalAxis", true);
            double distance = settings.getDouble("transformGizmo.distanceToCamera", 10.0F);

            // Draw the gizmos once for each camera that is active, which draws to a gizmos target.
            for (auto [entity, camera, cameraLtw, drawsTo, gizmosTarget] : cameraQuery)
            {
                if (!camera.active || entity == selection.entity)
                {
                    continue;
                }

                drawPositionGizmo(positionQuery, camera, cameraLtw, gizmos, input, window, selection.entity, useLocal,
                                  distance, entity);
                drawRotationGizmo(rotationQuery, camera, cameraLtw, gizmos, input, window, selection.entity, useLocal,
                                  distance, entity);
            }
        });
}
