#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>

using namespace cubos;

int main(void)
{
    initializeLogger();
    auto window = io::Window();
    auto& renderDevice = window.getRenderDevice();

    while (!window.shouldClose())
    {
        renderDevice.clearColor(0.0, 0.0, 0.0, 0.0f);
        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
