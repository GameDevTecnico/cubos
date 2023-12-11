#include <cubos/engine/utils/free_camera_controller/plugin.hpp>

using namespace cubos::engine;
using namespace cubos::core::io;

using cubos::engine::DeltaTime;

static void init(Write<Input> input)
{
    InputBindings bindings;
    bindings.axes()["free-horizontal-x"] = InputAxis{{{Key::W, Modifiers::None}, {Key::Up, Modifiers::None}},
                                                     {{Key::S, Modifiers::None}, {Key::Down, Modifiers::None}},
                                                     {}};
    bindings.axes()["free-horizontal-z"] = InputAxis{{{Key::D, Modifiers::None}, {Key::Right, Modifiers::None}},
                                                     {{Key::A, Modifiers::None}, {Key::Left, Modifiers::None}},
                                                     {}};
    bindings.axes()["free-vertical"] =
        InputAxis{{{Key::Space, Modifiers::None}}, {{Key::LControl, Modifiers::None}}, {}};

    input->bind(bindings);
}

static void updateSimpleController(Query<Read<SimpleController>, Write<Position>> entities, Read<DeltaTime> deltaTime)
{
    for (auto [entity, controller, position] : entities)
    {
        position->vec.y += controller->verticalAxis * deltaTime->value * controller->speed;

        position->vec.x += controller->horizontalXAxis * deltaTime->value * controller->speed;
        position->vec.z += controller->horizontalXAxis * deltaTime->value * controller->speed;

        position->vec.z += controller->horizontalZAxis * deltaTime->value * controller->speed;
        position->vec.x -= controller->horizontalZAxis * deltaTime->value * controller->speed;
    }
}

static void moveFreeController(glm::vec3& direction, Write<Position> position, Read<FreeController> controller,
                               Read<DeltaTime> deltaTime)
{
    position->vec.y += controller->verticalAxis * deltaTime->value * controller->speed;

    position->vec.x += direction.x * controller->horizontalXAxis * deltaTime->value * controller->speed;
    position->vec.z += direction.z * controller->horizontalXAxis * deltaTime->value * controller->speed;
    position->vec.y += direction.y * controller->horizontalXAxis * deltaTime->value * controller->speed;

    position->vec.z += direction.x * controller->horizontalZAxis * deltaTime->value * controller->speed;
    position->vec.x -= direction.z * controller->horizontalZAxis * deltaTime->value * controller->speed;
}

static void updateFreeController(Query<Read<FreeController>, Write<Position>, Write<Rotation>> entities,
                                 Read<DeltaTime> deltaTime)
{
    glm::vec3 direction;
    for (auto [entity, controller, position, rotation] : entities)
    {
        direction.x = glm::cos(glm::radians(controller->phi)) * glm::sin(glm::radians(controller->theta));
        direction.y = glm::sin(glm::radians(controller->phi));
        direction.z = glm::cos(glm::radians(controller->phi)) * glm::cos(glm::radians(controller->theta));

        direction = glm::normalize(direction);

        moveFreeController(direction, position, controller, deltaTime);

        rotation->quat = glm::quatLookAt(direction, glm::vec3{0.0F, 1.0F, 0.0F});
    }
}

static void processMouseMotion(EventReader<WindowEvent> windowEvent, Query<Write<FreeController>> entities,
                               Read<DeltaTime> deltaTime)
{
    glm::ivec2 delta;
    for (const auto& event : windowEvent)
    {
        for (auto [entity, controller] : entities)
        {
            if (std::holds_alternative<MouseMoveEvent>(event))
            {
                delta = std::get<MouseMoveEvent>(event).position - controller->lastMousePos;
                controller->lastMousePos = std::get<MouseMoveEvent>(event).position;
                controller->phi -= delta.y * deltaTime->value * controller->sens;
                controller->theta -= delta.x * deltaTime->value * controller->sens;
                controller->phi = std::clamp(controller->phi, -90.0F, 90.0F);
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

static void getAxes(Read<Input> input, Query<Write<FreeController>> entities)
{
    for (auto [entity, controller] : entities)
    {
        controller->verticalAxis = input->axis("free-vertical");
        controller->horizontalXAxis = input->axis("free-horizontal-x");
        controller->horizontalZAxis = input->axis("free-horizontal-z");
    }
}

void cubos::engine::freeCameraControllerPlugin(Cubos& cubos)
{
    cubos.addPlugin(inputPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.addComponent<SimpleController>();
    cubos.addComponent<FreeController>();

    cubos.startupSystem(lockMouseSystem).after("cubos.window.init");
    cubos.startupSystem(init).tagged("cubos.input");

    cubos.system(getAxes);
    cubos.system(updateSimpleController);
    cubos.system(processMouseMotion);
    cubos.system(updateFreeController);
}