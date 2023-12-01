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

static void updateSimpleController(Query<Read<SimpleController>, Write<Position>> entities, Read<DeltaTime> deltaTime)
{
    for(auto [entity, controller, position] : entities)
    {
        position->vec.y += controller->verticalAxis * deltaTime->value * controller->speed;

        position->vec.x += controller->horizontalXAxis * deltaTime->value * controller->speed;
        position->vec.z += controller->horizontalXAxis * deltaTime->value * controller->speed;

        position->vec.z += controller->horizontalZAxis * deltaTime->value * controller->speed;
        position->vec.x -= controller->horizontalZAxis * deltaTime->value * controller->speed;
    }
}

static void moveFreeController(glm::vec3& direction, Write<Position> position, Read<FreeController> controller, Read<DeltaTime> deltaTime)
{
        position->vec.y += controller->verticalAxis * deltaTime->value * controller->speed;

        position->vec.x += direction.x * controller->horizontalXAxis * deltaTime->value * controller->speed;
        position->vec.z += direction.z * controller->horizontalXAxis * deltaTime->value * controller->speed;
        position->vec.y += direction.y * controller->horizontalXAxis * deltaTime->value * controller->speed;

        position->vec.z += direction.x * controller->horizontalZAxis * deltaTime->value * controller->speed;
        position->vec.x -= direction.z * controller->horizontalZAxis * deltaTime->value * controller->speed;
}

static void updateFreeController(Query<Read<FreeController>, Write<Position>, Write<Rotation>> entities, Read<DeltaTime> deltaTime)
{
    glm::vec3 direction;
    for(auto [entity, controller, position, rotation] : entities)
    {
        direction.x = glm::cos(glm::radians(controller->phi)) * glm::sin(glm::radians(controller->theta));
        direction.y = glm::sin(glm::radians(controller->phi));
        direction.z = glm::cos(glm::radians(controller->phi)) * glm::cos(glm::radians(controller->theta));

        direction = glm::normalize(direction);

        moveFreeController(direction, position, controller, deltaTime);

        rotation->quat = glm::quatLookAt(direction, glm::vec3{0.0F, 1.0F, 0.0F});
    }
}

static void processMouseMotion(EventReader<WindowEvent> windowEvent, Query<Write<FreeController>> entities, Read<DeltaTime> deltaTime)
{
    glm::ivec2 delta;
    for(const auto& event : windowEvent)
    {
        for (auto [entity, controller] : entities)
        {
            if(std::holds_alternative<MouseMoveEvent>(event))
            {
                delta = std::get<MouseMoveEvent>(event).position - controller->lastMousePos;
                controller->lastMousePos = std::get<MouseMoveEvent>(event).position;
                controller->phi -= delta.y * deltaTime->value * controller->sens;
                controller->theta -= delta.x * deltaTime->value * controller->sens;
                controller->phi = std::clamp(controller->phi, -90.0f, 90.0f);
                delta = glm::ivec2{0, 0};
            }
        }
    }
}

static void lockMouseSystem(Write<Window> window, Query<Write<FreeController>> entities)
{
    (*window)->mouseState(MouseState::Locked);
    for (auto [entity, controller] : entities)
    {
        controller->lastMousePos = (*window)->getMousePosition();
    }
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
    cubos.system(updateFreeController);
}