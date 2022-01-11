#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/input/input_manager.hpp>

using namespace cubos;
float red = 0.0f;
void redIinc(input::InputContext context)
{
    red += 0.1f;
    if (red > 1)
    {
        red = 1;
    }
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

    input::InputManager::init(window);
    auto incRedAction = input::InputManager::createAction("increase_red");
    incRedAction->addBinding(redIinc);
    incRedAction->addInput(new input::ButtonPress(io::Key::W));
    incRedAction->addInput(new input::ButtonPress(io::MouseButton::Left));

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
