#include <cubos/cubos.hpp>
#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>

void cubos::init(int argc, char** argv)
{
    initializeLogger();
}

void cubos::run()
{
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    while (!window->shouldClose())
    {
        renderDevice.clearColor(0.0, 0.0, 0.0, 0.0f);
        window->swapBuffers();
        window->pollEvents();
    }

    delete window;
}
