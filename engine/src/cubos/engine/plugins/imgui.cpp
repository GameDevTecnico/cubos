#include <cubos/engine/plugins/imgui.hpp>
#include <cubos/engine/plugins/window.hpp>
#include <cubos/core/ui/imgui.hpp>

static void initializeImGui(const cubos::core::io::Window& window)
{
    cubos::core::ui::initialize(window);
}

static void beginImGuiFrame()
{
    cubos::core::ui::beginFrame();
}

static void endImGuiFrame()
{
    cubos::core::ui::endFrame();
}

void cubos::engine::plugins::imguiPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::plugins::windowPlugin);

    cubos.startupTag("InitImGui")
        .afterTag("OpenWindow");

    cubos.tag("BeginImGuiFrame")
        .afterTag("Poll");

    cubos.tag("EndImGuiFrame")
        .beforeTag("SwapBuffers");

    cubos.startupSystem(initializeImGui)
        .tagged("InitImGui");

    cubos.system(beginImGuiFrame)
        .tagged("BeginImGuiFrame");

    cubos.system(endImGuiFrame)
        .tagged("EndImGuiFrame");
}
