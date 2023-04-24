#include <cubos/core/ui/imgui.hpp>

#include <cubos/engine/plugins/imgui.hpp>
#include <cubos/engine/plugins/window.hpp>

using namespace cubos::core;

static void init(const io::Window& window)
{
    ui::initialize(window);
}

static void begin(ecs::EventReader<io::WindowEvent> events)
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

static void end()
{
    ui::endFrame();
}

void cubos::engine::plugins::imguiPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::plugins::windowPlugin);

    cubos.startupTag("cubos.imgui.init").afterTag("cubos.window.init");
    cubos.tag("cubos.imgui.begin").afterTag("cubos.window.poll");
    cubos.tag("cubos.imgui.end").beforeTag("cubos.window.render").afterTag("cubos.imgui.begin");
    cubos.tag("cubos.imgui").afterTag("cubos.imgui.begin").beforeTag("cubos.imgui.end");

    cubos.startupSystem(init).tagged("cubos.imgui.init");
    cubos.system(begin).tagged("cubos.imgui.begin");
    cubos.system(end).tagged("cubos.imgui.end");
}
