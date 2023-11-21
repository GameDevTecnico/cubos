#include <cubos/engine/utils/free_camera_controller/plugin.hpp>
#include <cubos/core/data/old/debug_serializer.hpp>

using namespace cubos::engine;
using namespace cubos::core::io;

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::ecs::EventReader;
using cubos::engine::DeltaTime;
using cubos::core::data::old::Debug;

glm::ivec2 lastMousePos;
glm::ivec2 delta = {0, 0};
glm::vec3 direction = {1.0f, 0.0f, 1.0f};
int counter = 0;

static void updateSimpleController(Query<Read<SimpleController>, Write<Position>> entities, Read<DeltaTime> deltaTime, Write<Window> window)
{
    CUBOS_INFO("Mouse Pos x: {}", Debug((*window)->getMousePosition()));
    for(auto [entity, controller, position] : entities)
    {
        position->vec.y += controller->verticalAxis * deltaTime->value * controller->speed;

        position->vec.x += controller->horizontalXAxis * deltaTime->value * controller->speed;
        position->vec.z += controller->horizontalXAxis * deltaTime->value * controller->speed;

        position->vec.z += controller->horizontalZAxis * deltaTime->value * controller->speed;
        position->vec.x -= controller->horizontalZAxis * deltaTime->value * controller->speed;
    }
}

static void updateFreeController(Query<Read<FreeController>, Write<Position>, Write<Rotation>> entities)
{
    for(auto [entity, controller, position, rotation] : entities)
    {
        if (delta != glm::ivec2{0, 0})
        {
            delta = glm::ivec2{0, 0};
            direction.x = glm::sin(glm::radians(controller->phi)) * glm::sin(glm::radians(controller->theta));
            direction.y = glm::cos(glm::radians(controller->phi));
            direction.z = glm::sin(glm::radians(controller->phi)) * glm::cos(glm::radians(controller->theta));
        }

        rotation->quat = glm::quatLookAt(glm::normalize(direction), glm::vec3{0.0F, 1.0F, 0.0F});
    }
}

static void processMouseMotion(EventReader<WindowEvent> windowEvent)
{
    for(const auto& event : windowEvent)
    {
        if(std::holds_alternative<MouseMoveEvent>(event))
        {
            delta = std::get<MouseMoveEvent>(event).position - lastMousePos;
            lastMousePos = std::get<MouseMoveEvent>(event).position;
        }
    }
}

static void processDelta(Query<Write<FreeController>> entities, Read<DeltaTime> deltaTime)
{
    if (delta.x > 0)
    {
        for (auto [entity, controller] : entities)
        {
            controller->phi +=  deltaTime->value * controller->sens;
        }
    } else if (delta.x < 0)
    {
        for (auto [entity, controller] : entities)
        {
            controller->phi -= deltaTime->value * controller->sens;
        }
    }

    if (delta.y > 0)
    {
        for (auto [entity, controller] : entities)
        {
            controller->theta += deltaTime->value * controller->sens;
        }
    } else if (delta.y < 0)
    {
        for (auto [entity, controller] : entities)
        {
            controller->theta -= deltaTime->value * controller->sens;
        }
    }
}

static void lockMouseSystem(Write<Window> window)
{
    (*window)->mouseState(MouseState::Locked);
    lastMousePos = (*window)->getMousePosition();
}

void updateFreeVertical(float vertical, cubos::core::ecs::Write<cubos::engine::FreeController> controller)
{
    controller->verticalAxis = vertical;
}

void updateFreeHorizontalX(float horizontalX, cubos::core::ecs::Write<cubos::engine::FreeController> controller)
{
    controller->horizontalXAxis = horizontalX;
}

void updateFreeHorizontalZ(float horizontalZ, cubos::core::ecs::Write<cubos::engine::FreeController> controller)
{
    controller->horizontalZAxis = horizontalZ;
}

void updateSimpleVertical(float vertical, cubos::core::ecs::Write<cubos::engine::SimpleController> controller)
{
    controller->verticalAxis = vertical;
}

void updateSimpleHorizontalX(float horizontalX, cubos::core::ecs::Write<cubos::engine::SimpleController> controller)
{
    controller->horizontalXAxis = horizontalX;
}

void updateSimpleHorizontalZ(float horizontalZ, cubos::core::ecs::Write<cubos::engine::SimpleController> controller)
{
    controller->horizontalZAxis = horizontalZ;
}

void cubos::engine::freeCameraControllerPlugin(Cubos& cubos)
{
    cubos.addPlugin(windowPlugin);

    cubos.addComponent<SimpleController>();
    cubos.addComponent<FreeController>();

    cubos.startupSystem(lockMouseSystem).after("cubos.window.init");

    cubos.system(updateSimpleController);
    cubos.system(processMouseMotion);
    cubos.system(processDelta);
    cubos.system(updateFreeController);
}