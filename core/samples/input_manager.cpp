#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/input/input_manager.hpp>
#include <cubos/input/input_sources/button_press.hpp>
#include <cubos/input/input_sources/single_axis.hpp>
#include <cubos/input/input_sources/double_axis.hpp>
#include <algorithm>
using namespace cubos;
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int width = 0;
int height = 0;

void updateColour(input::InputContext context)
{
    red = std::clamp(context.getValue<glm::vec2>().x / width, 0.0f, 1.0f);
    green = std::clamp(context.getValue<glm::vec2>().y / width, 0.0f, 1.0f);
}

void blueIncr(input::InputContext context)
{
    blue += 0.1f;
    if (blue < 0)
    {
        blue = 0;
    }
}

void blueDecr(input::InputContext context)
{
    blue -= 0.1f;
    if (blue < 0)
    {
        blue = 0;
    }
}
int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    height = window->getFramebufferSize().y;
    width = window->getFramebufferSize().x;

    input::InputManager::init(window);
    auto updateRedGreenAction = input::InputManager::createAction("update_red_green");
    updateRedGreenAction->addBinding(updateColour);
    updateRedGreenAction->addInput(new input::DoubleAxis(cubos::io::MouseAxis::X, cubos::io::MouseAxis::Y));

    auto incBlueAction = input::InputManager::createAction("increase_blue");
    incBlueAction->addBinding(blueIncr);
    incBlueAction->addInput(new input::ButtonPress(io::Key::D));
    incBlueAction->addInput(new input::ButtonPress(io::MouseButton::Right));

    auto decBlueAction = input::InputManager::createAction("decrease_blue");
    decBlueAction->addBinding(blueDecr);
    decBlueAction->addInput(new input::ButtonPress(io::Key::A));
    decBlueAction->addInput(new input::ButtonPress(io::MouseButton::Left));

    while (!window->shouldClose())
    {
        renderDevice.clearColor(red, green, blue, 0.0f);
        window->swapBuffers();
        window->pollEvents();
        input::InputManager::processActions();
    }

    delete window;
    return 0;
}
