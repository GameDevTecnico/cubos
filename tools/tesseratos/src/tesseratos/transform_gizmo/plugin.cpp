#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include <tesseratos/debug_camera/plugin.hpp>
#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/transform_gizmo/plugin.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::EventReader;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;
using cubos::core::io::MouseMoveEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

using cubos::engine::ActiveCameras;
using cubos::engine::Camera;
using cubos::engine::Cubos;
using cubos::engine::Gizmos;
using cubos::engine::Position;
using cubos::engine::Rotation;
using cubos::engine::Scale;

using namespace tesseratos;

glm::ivec2 lastMousePos;

glm::vec3 intersectLinePlane(glm::vec3 linePoint, glm::vec3 lineVector, glm::vec3 planePoint, glm::vec3 planeNormal)
{
    if (float distance; glm::intersectRayPlane(linePoint, lineVector, planePoint, planeNormal, distance))
    {
        return linePoint + lineVector * distance;
    }
    else if (glm::intersectRayPlane(linePoint, -lineVector, planePoint, planeNormal, distance))
    {
        return linePoint - lineVector * distance;
    }
    return {0, 0, 0};
}

glm::vec3 mouseWorldDirection(glm::ivec2 windowSize, glm::ivec2 mousePos, const glm::mat4& pv)
{
    float mouseX = (float)mousePos.x / ((float)windowSize.x * 0.5f) - 1.0f;
    float mouseY = (float)mousePos.y / ((float)windowSize.y * 0.5f) - 1.0f;

    return glm::normalize(glm::vec3(glm::inverse(pv) * glm::vec4(mouseX, -mouseY, 1.0f, 1.0f)));
}

glm::vec3 mouseWorldCoordinates(glm::ivec2 windowSize, glm::ivec2 mousePos, glm::vec3 transformPosition,
                                glm::vec3 transformNormal, glm::vec3 cameraPosition, const glm::mat4& pv)
{
    auto dir = mouseWorldDirection(windowSize, mousePos, pv);
    return intersectLinePlane(cameraPosition, dir, transformPosition, transformNormal);
}

void checkMovement(Position& position, glm::vec3 transformVector, glm::vec3 transformNormal, glm::ivec2 windowSize,
                   glm::ivec2 mousePos, const glm::mat4& pv, glm::vec3 cameraPosition)
{
    auto mousePosWorld = mouseWorldCoordinates(windowSize, mousePos, position.vec, transformNormal, cameraPosition, pv);
    auto oldMousePosWorld =
        mouseWorldCoordinates(windowSize, lastMousePos, position.vec, transformNormal, cameraPosition, pv);
    position.vec += (mousePosWorld - oldMousePosWorld) * transformVector;
}

void drawTransformGizmo(World& world, Entity cameraEntity, Gizmos& gizmos, EventReader<WindowEvent>& windowEvent,
                        Position& position, const Camera& camera, const Window& window)
{
    auto cameraPosition = glm::vec3(0.0F);
    if (world.components(cameraEntity).has<Position>())
    {
        cameraPosition = world.components(cameraEntity).get<Position>().vec;
    }

    auto pv = glm::mat4(1.0F);
    if (world.components(cameraEntity).has<Rotation>())
    {
        pv *= glm::toMat4(world.components(cameraEntity).get<Rotation>().quat);
    }

    if (world.components(cameraEntity).has<Scale>())
    {
        pv = glm::scale(pv, glm::vec3(world.components(cameraEntity).get<Scale>().factor));
    }
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

    glm::ivec2 newMousePos = lastMousePos;
    for (const auto& event : windowEvent)
    {
        if (std::holds_alternative<MouseMoveEvent>(event))
        {
            newMousePos = std::get<MouseMoveEvent>(event).position;
        }
    }

    // Axis movement
    if (gizmos.locked("transform_gizmo.x"))
    {
        checkMovement(position, {1, 0, 0}, {0, 1, 0}, window->size(), newMousePos, pv, cameraPosition);
        xColor = {1, 1, 0};
    }
    if (gizmos.locked("transform_gizmo.y"))
    {
        checkMovement(position, {0, 1, 0}, {1, 0, 0}, window->size(), newMousePos, pv, cameraPosition);
        yColor = {1, 1, 0};
    }
    if (gizmos.locked("transform_gizmo.z"))
    {
        checkMovement(position, {0, 0, 1}, {0, 1, 0}, window->size(), newMousePos, pv, cameraPosition);
        zColor = {1, 1, 0};
    }

    // Planar movement
    if (gizmos.locked("transform_gizmo.xy"))
    {
        checkMovement(position, {1, 1, 0}, {0, 0, 1}, window->size(), newMousePos, pv, cameraPosition);
        xyColor = {1, 1, 1};
    }
    if (gizmos.locked("transform_gizmo.xz"))
    {
        checkMovement(position, {1, 0, 1}, {0, 1, 0}, window->size(), newMousePos, pv, cameraPosition);
        xzColor = {1, 1, 1};
    }
    if (gizmos.locked("transform_gizmo.yz"))
    {
        checkMovement(position, {0, 1, 1}, {1, 0, 0}, window->size(), newMousePos, pv, cameraPosition);
        yzColor = {1, 1, 1};
    }

    gizmos.color(xyColor);
    gizmos.drawBox("transform_gizmo.xy", position.vec - glm::vec3{0, 0, 0.015F},
                   position.vec + glm::vec3{0.25F, 0.25F, 0.015F});
    gizmos.color(xzColor);
    gizmos.drawBox("transform_gizmo.xz", position.vec - glm::vec3{0, 0.015F, 0},
                   position.vec + glm::vec3{0.25F, 0.015F, 0.25F});
    gizmos.color(yzColor);
    gizmos.drawBox("transform_gizmo.yz", position.vec - glm::vec3{0.015F, 0, 0},
                   position.vec + glm::vec3{0.015F, 0.25F, 0.25F});

    gizmos.color({0.7F, 0.7F, 0.7F});
    gizmos.drawBox("", position.vec + glm::vec3{0.03F, 0.03F, 0.03F}, position.vec - glm::vec3{0.03F, 0.03F, 0.03F});

    gizmos.color(xColor);
    gizmos.drawArrow("transform_gizmo.x", position.vec + glm::vec3{0.03f, 0, 0}, {1, 0, 0}, 0.03F, 0.07F, 0.7F);
    gizmos.color(yColor);
    gizmos.drawArrow("transform_gizmo.y", position.vec + glm::vec3{0, 0.03f, 0}, {0, 1, 0}, 0.03F, 0.07F, 0.7F);
    gizmos.color(zColor);
    gizmos.drawArrow("transform_gizmo.z", position.vec + glm::vec3{0, 0, 0.03f}, {0, 0, 1}, 0.03F, 0.07F, 0.7F);

    lastMousePos = newMousePos;
}

static void gizmoSystem(Write<World> world, EventReader<WindowEvent> windowEvent)
{
    const EntitySelector& entitySelector = world->read<EntitySelector>().get();
    const ActiveCameras& activeCameras = world->read<ActiveCameras>().get();
    const Window& window = world->read<Window>().get();

    Gizmos& gizmos = world->write<Gizmos>().get();

    if (entitySelector.selection.isNull())
    {
        return;
    }
    if (activeCameras.entities[0] == entitySelector.selection)
    {
        return;
    }
    if (!world->components(entitySelector.selection).has<Position>())
    {
        return;
    }

    const Camera& camera = world->components(activeCameras.entities[0]).get<Camera>();

    Position& position = world->components(entitySelector.selection).get<Position>();

    drawTransformGizmo(*world, activeCameras.entities[0], gizmos, windowEvent, position, camera, window);
}

void tesseratos::transformGizmoPlugin(Cubos& cubos)
{
    cubos.addPlugin(entitySelectorPlugin);
    cubos.addPlugin(cubos::engine::transformPlugin);
    cubos.addPlugin(cubos::engine::gizmosPlugin);

    cubos.system(gizmoSystem).tagged("cubos.imgui");
}