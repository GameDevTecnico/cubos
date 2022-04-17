#include <cubos/cubos.hpp>
#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>

void cubos::engine::init(int argc, char** argv)
{
  core::initializeLogger();
}

void cubos::engine::run()
{
    auto window = core::io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    while (!window->shouldClose())
    {
        renderDevice.clearColor(0.0, 0.0, 0.0, 0.0f);
        window->swapBuffers();
        window->pollEvents();
    }

    delete window;
}
