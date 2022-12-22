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
    cubos.addPlugin(cubos::engine::plugins::windowPlugin)
        .startupTag("InitImGui").afterTag("OpenWindow")
        .tag("BeginImGuiFrame").afterTag("Poll")
        .tag("EndImGuiFrame").beforeTag("SwapBuffers")

        .startupSystem(initializeImGui).tagged("InitImGui")

        .system(beginImGuiFrame).tagged("BeginImGuiFrame")

        .system(endImGuiFrame).tagged("EndImGuiFrame");
}
