#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/input/input_manager.hpp>
#include <algorithm>
using namespace cubos;
float red = 0.0f;
int width = 0;
int height = 0;

void updateRed(input::InputContext context)
{
    red = std::clamp(context.getValue().x / width, 0.0f, 1.0f);
}

void redIdec(input::InputContext context)
{
    red -= 0.1f;
    if (red < 0)
    {
        red = 0;
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
    auto incRedAction = input::InputManager::createAction("update_red");
    incRedAction->addBinding(updateRed);
    incRedAction->addInput(new input::DoubleAxis(cubos::io::MouseAxis::X, cubos::io::MouseAxis::Y));

    auto decRedAction = input::InputManager::createAction("decrease_red");
    decRedAction->addBinding(redIdec);
    decRedAction->addInput(new input::ButtonPress(io::Key::S));
    decRedAction->addInput(new input::ButtonPress(io::MouseButton::Right));

    while (!window->shouldClose())
    {
        renderDevice.clearColor(red, 0.0, 0.0, 0.0f);
        window->swapBuffers();
        window->pollEvents();
        input::InputManager::processActions();
    }

    delete window;
    return 0;
}
