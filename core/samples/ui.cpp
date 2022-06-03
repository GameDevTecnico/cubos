#include <cubos/core/log.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/ui/imgui.hpp>

#include <imgui.h>

using namespace cubos::core;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    ui::initialize(*window);

    while (!window->shouldClose())
    {
        ui::beginFrame();

        ImGui::ShowDemoWindow();

        auto sz = window->getFramebufferSize();
        renderDevice.setRasterState(nullptr);
        renderDevice.setBlendState(nullptr);
        renderDevice.setDepthStencilState(nullptr);
        renderDevice.setViewport(0, 0, sz.x, sz.y);
        renderDevice.clearColor(0.0f, 0.0f, 0.0f, 1.0f);

        ui::endFrame();
        window->swapBuffers();
        window->pollEvents();
    }

    ui::terminate();
    delete window;
    return 0;
}
