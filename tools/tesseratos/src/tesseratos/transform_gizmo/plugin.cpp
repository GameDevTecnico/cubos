#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include <tesseratos/debug_camera/plugin.hpp>
#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/transform_gizmo/plugin.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::EventReader;
using cubos::core::ecs::Opt;
using cubos::core::ecs::Query;
using cubos::core::ecs::World;
using cubos::core::io::Window;

using cubos::engine::ActiveCameras;
using cubos::engine::Camera;
using cubos::engine::Cubos;
using cubos::engine::Gizmos;
using cubos::engine::Input;
using cubos::engine::LocalToWorld;
using cubos::engine::Position;
using cubos::engine::Rotation;
using cubos::engine::Scale;
using cubos::engine::Settings;

using namespace tesseratos;

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

static void drawPositionGizmo(Query<const Camera&, LocalToWorld&> cameraQuery,
                              Query<Position&, LocalToWorld&> positionQuery, Gizmos& gizmos, const Input& input,
                              const Window& window, Entity cameraEntity, Entity selectedEntity, bool useLocalAxis)
{
    auto [camera, cameraLtw] = *cameraQuery.at(cameraEntity);

    auto [position, localToWorld] = *positionQuery.at(selectedEntity);

    auto cameraPosition = cameraLtw.worldPosition();

    auto pv = cameraLtw.mat;

    pv = glm::inverse(pv);

    pv = glm::perspective(glm::radians(camera.fovY), (float)window->size().x / (float)window->size().y, camera.zNear,
                          camera.zFar) *
         pv;

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

    if (!useLocalAxis)
    {
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

        gizmos.color(xyColor);
        gizmos.drawBox("transform_gizmo.position.xy", localToWorld.worldPosition() - glm::vec3{0, 0, 0.015F},
                       localToWorld.worldPosition() + glm::vec3{0.25F, 0.25F, 0.015F});
        gizmos.color(xzColor);
        gizmos.drawBox("transform_gizmo.position.xz", localToWorld.worldPosition() - glm::vec3{0, 0.015F, 0},
                       localToWorld.worldPosition() + glm::vec3{0.25F, 0.015F, 0.25F});
        gizmos.color(yzColor);
        gizmos.drawBox("transform_gizmo.position.yz", localToWorld.worldPosition() - glm::vec3{0.015F, 0, 0},
                       localToWorld.worldPosition() + glm::vec3{0.015F, 0.25F, 0.25F});
    }

    gizmos.color({0.7F, 0.7F, 0.7F});
    gizmos.drawBox("", localToWorld.worldPosition() + glm::vec3{0.03F, 0.03F, 0.03F},
                   localToWorld.worldPosition() - glm::vec3{0.03F, 0.03F, 0.03F});

    gizmos.color(xColor);
    gizmos.drawArrow("transform_gizmo.position.x", localToWorld.worldPosition() + rightVector * 0.03F, rightVector,
                     0.03F, 0.07F, 0.7F);
    gizmos.color(yColor);
    gizmos.drawArrow("transform_gizmo.position.y", localToWorld.worldPosition() + upVector * 0.03F, upVector, 0.03F,
                     0.07F, 0.7F);
    gizmos.color(zColor);
    gizmos.drawArrow("transform_gizmo.position.z", localToWorld.worldPosition() + forwardVector * 0.03F, forwardVector,
                     0.03F, 0.07F, 0.7F);
}

static void drawRotationGizmo(Query<const Camera&, LocalToWorld&> cameraQuery,
                              Query<Rotation&, LocalToWorld&> positionQuery, Gizmos& gizmos, const Input& input,
                              const Window& window, Entity cameraEntity, Entity selectedEntity, bool useLocalAxis)
{
    auto [camera, cameraLtw] = *cameraQuery.at(cameraEntity);

    auto [rotation, localToWorld] = *positionQuery.at(selectedEntity);

    auto cameraPosition = cameraLtw.worldPosition();

    auto pv = cameraLtw.mat;

    pv = glm::inverse(pv);

    pv = glm::perspective(glm::radians(camera.fovY), (float)window->size().x / (float)window->size().y, camera.zNear,
                          camera.zFar) *
         pv;

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

    gizmos.color(xColor);
    gizmos.drawRing("transform_gizmo.rotation.x", localToWorld.worldPosition() + (rightVector * 0.1F),
                    localToWorld.worldPosition() - (rightVector * 0.01F), 1.4F, 1.25F);
    gizmos.color(yColor);
    gizmos.drawRing("transform_gizmo.rotation.y", localToWorld.worldPosition() + (upVector * 0.1F),
                    localToWorld.worldPosition() - (upVector * 0.01F), 1.4F, 1.25F);
    gizmos.color(zColor);
    gizmos.drawRing("transform_gizmo.rotation.z", localToWorld.worldPosition() + (forwardVector * 0.1F),
                    localToWorld.worldPosition() - (forwardVector * 0.01F), 1.4F, 1.25F);
}

void tesseratos::transformGizmoPlugin(Cubos& cubos)
{
    cubos.addPlugin(entitySelectorPlugin);
    cubos.addPlugin(cubos::engine::transformPlugin);
    cubos.addPlugin(cubos::engine::gizmosPlugin);
    cubos.addPlugin(cubos::engine::inputPlugin);
    cubos.addPlugin(cubos::engine::settingsPlugin);

    cubos.system("draw Transform Gizmo")
        .after("cubos.transform.update")
        .tagged("cubos.imgui")
        .call([](const EntitySelector& entitySelector, const ActiveCameras& activeCameras, const Window& window,
                 const Input& input, Settings& settings, Gizmos& gizmos, Query<Position&, LocalToWorld&> positionQuery,
                 Query<Rotation&, LocalToWorld&> rotationQuery, Query<const Camera&, LocalToWorld&> cameraQuery) {
            if (entitySelector.selection.isNull())
            {
                return;
            }
            if (activeCameras.entities[0] == entitySelector.selection)
            {
                return;
            }
            if (!positionQuery.at(entitySelector.selection))
            {
                return;
            }
            if (!cameraQuery.at(activeCameras.entities[0]))
            {
                return;
            }

            bool useLocal = settings.getBool("transformGizmo.useLocalAxis", true);

            drawPositionGizmo(cameraQuery, positionQuery, gizmos, input, window, activeCameras.entities[0],
                              entitySelector.selection, useLocal);
            drawRotationGizmo(cameraQuery, rotationQuery, gizmos, input, window, activeCameras.entities[0],
                              entitySelector.selection, useLocal);
        });
}