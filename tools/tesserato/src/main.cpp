#include <cubos/engine/cubos.hpp>

#include <cubos/engine/plugins/window.hpp>
#include <cubos/engine/plugins/env_settings.hpp>

#include <cubos/core/ui/imgui.hpp>
#include <cubos/core/ui/serialization.hpp>
#include <imgui.h>

static void setup(const cubos::core::io::Window& window, ShouldQuit& quit)
{
    cubos::core::ui::initialize(window);
}

static void show(const cubos::core::io::Window& window)
{
    auto& renderDevice = window->getRenderDevice();

    cubos::core::ui::beginFrame();

    ImGui::Begin("Editor");
    ImGui::End();

    // ImGui::ShowDemoWindow();

    auto sz = window->getFramebufferSize();
    renderDevice.setRasterState(nullptr);
    renderDevice.setBlendState(nullptr);
    renderDevice.setDepthStencilState(nullptr);
    renderDevice.setViewport(0, 0, sz.x, sz.y);
    renderDevice.clearColor(0.0f, 0.0f, 0.0f, 1.0f);

    cubos::core::ui::endFrame();
}

int main(int argc, char** argv)
{
    Cubos(argc, argv)
        .addPlugin(cubos::engine::plugins::windowPlugin)
        .addStartupSystem(setup, "SetupUI")
        .addSystem(show, "showUI")
        .run();
}
