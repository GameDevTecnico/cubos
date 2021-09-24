#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>

#include <cubos/log.hpp>

using namespace cubos;

int main(void)
{
    initializeLogger();
    auto window = cubos::io::Window();
    auto& renderDevice = window.getRenderDevice();

    while (!window.shouldClose())
    {
        renderDevice.clearColor(0.894f, 0.592f, 0.141f, 0.0f);
        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
