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

static void getInitRotation(Query<Write<FreeCameraController>, Read<Rotation>> entities)
{
    for (auto [entity, controller, rotation] : entities)
    {
        glm::vec3 fw_vector = rotation->quat * glm::vec3(0.0F, 0.0F, -1.0F);
        controller->phi = glm::degrees(glm::asin(fw_vector.y));
        controller->theta = glm::degrees(glm::atan(fw_vector.x, fw_vector.z));
    }
}

static void moveFreeController(Read<Input> input, glm::vec3& direction, Write<Position> position,
                               Read<FreeCameraController> controller, Read<DeltaTime> deltaTime)
{
    position->vec.y += input->axis("free-vertical") * deltaTime->value * controller->speed;

    position->vec.x += direction.x * input->axis("free-horizontal-x") * deltaTime->value * controller->speed;
    position->vec.z += direction.z * input->axis("free-horizontal-x") * deltaTime->value * controller->speed;
    position->vec.y += direction.y * input->axis("free-horizontal-x") * deltaTime->value * controller->speed;

    position->vec.z += direction.x * input->axis("free-horizontal-z") * deltaTime->value * controller->speed;
    position->vec.x -= direction.z * input->axis("free-horizontal-z") * deltaTime->value * controller->speed;
}

static void updateFreeController(Read<Input> input,
                                 Query<Read<FreeCameraController>, Write<Position>, Write<Rotation>> entities,
                                 Read<DeltaTime> deltaTime)
{
    glm::vec3 direction;
    for (auto [entity, controller, position, rotation] : entities)
    {
        direction.x = glm::cos(glm::radians(controller->phi)) * glm::sin(glm::radians(controller->theta));
        direction.y = glm::sin(glm::radians(controller->phi));
        direction.z = glm::cos(glm::radians(controller->phi)) * glm::cos(glm::radians(controller->theta));

        printf("%f %f %f\n", direction.x, direction.y, direction.z);
        direction = glm::normalize(direction);

        moveFreeController(input, direction, position, controller, deltaTime);

        rotation->quat = glm::quatLookAt(direction, glm::vec3{0.0F, 1.0F, 0.0F});
    }
}

static void processMouseMotion(EventReader<WindowEvent> windowEvent, Query<Write<FreeCameraController>> entities,
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
                controller->phi -= static_cast<float>(delta.y) * deltaTime->value * controller->sens;
                controller->theta -= static_cast<float>(delta.x) * deltaTime->value * controller->sens;
                controller->phi = std::clamp(controller->phi, -90.0F, 90.0F);
                delta = glm::ivec2{0, 0};
            }
        }
    }
}

static void lockMouseSystem(Write<Window> window, Query<Write<FreeCameraController>> entities)
{
    (*window)->mouseState(MouseState::Locked);
    for (auto [entity, controller] : entities)
    {
        controller->lastMousePos = (*window)->getMousePosition();
    }
}

void cubos::engine::freeCameraControllerPlugin(Cubos& cubos)
{
    cubos.addPlugin(inputPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.addComponent<FreeCameraController>();

    cubos.startupSystem(lockMouseSystem).after("cubos.window.init");
    cubos.startupSystem(init).tagged("cubos.input");
    cubos.startupSystem(getInitRotation);

    cubos.system(processMouseMotion);
    cubos.system(updateFreeController);
}