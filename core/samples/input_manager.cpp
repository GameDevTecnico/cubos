#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/input/input_manager.hpp>

using namespace cubos;
float red = 0.0f;
void redIinc()
{
    red += 0.1f;
}

void redIdec()
{
    red -= 0.1f;
}
int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    /*
    input::InputManager::Init(window);
    input::InputManager::RegisterMouseBinding(cubos::io::MouseButton::Left, redIinc);
    input::InputManager::RegisterKeyBinding(cubos::io::Key::D, redIdec);
    */
    while (!window->shouldClose())
    {
        renderDevice.clearColor(red, 0.0, 0.0, 0.0f);
        window->swapBuffers();
        window->pollEvents();
    }

    delete window;
    return 0;
}
