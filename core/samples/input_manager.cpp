#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/input/input_manager.hpp>

using namespace cubos;
float red = 0.0f;
void redIinc(input::InputContext context)
{
    red += 0.1f;
}

void redIdec(input::InputContext context)
{
    red -= 0.1f;
}
int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    input::InputManager::Init(window);
    auto testAction = input::InputManager::CreateAction("test");
    testAction->AddBinding(redIinc);
    testAction->AddInput(new input::ButtonPress(io::Key::K));

    while (!window->shouldClose())
    {
        renderDevice.clearColor(red, 0.0, 0.0, 0.0f);
        window->swapBuffers();
        window->pollEvents();
        input::InputManager::ProcessActions();
    }

    delete window;
    return 0;
}
