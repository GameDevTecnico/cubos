#include <cubos/engine/plugins/imgui.hpp>
#include <cubos/engine/plugins/window.hpp>
#include <cubos/core/ui/imgui.hpp>

using namespace cubos::core;

static void initializeImGui(const io::Window& window)
{
    ui::initialize(window);
}

static void beginImGuiFrame(ecs::EventReader<io::WindowEvent> events)
{
    // Pass window events to ImGui.
    // TODO: handleEvent returns a bool indicating if the event was handled or not.
    //       This will be used to stop propagating the event to other systems when
    //       the mouse is over an ImGui window.
    //       Not sure how we will propagate that information to other systems yet.
    for (auto event : events)
        ui::handleEvent(event);
    ui::beginFrame();
}

static void endImGuiFrame()
{
    ui::endFrame();
}

void cubos::engine::plugins::imguiPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::plugins::windowPlugin);

    cubos.startupTag("InitImGui").afterTag("OpenWindow");

    cubos.tag("BeginImGuiFrame").afterTag("Poll");

    cubos.tag("EndImGuiFrame").beforeTag("SwapBuffers");

    cubos.startupSystem(initializeImGui).tagged("InitImGui");

    cubos.system(beginImGuiFrame).tagged("BeginImGuiFrame");

    cubos.system(endImGuiFrame).tagged("EndImGuiFrame");
}
