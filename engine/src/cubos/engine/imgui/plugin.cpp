#include <cubos/core/ui/imgui.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::EventReader;
using cubos::core::ecs::Read;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;
namespace ui = cubos::core::ui;

static void init(Read<Window> window)
{
    ui::initialize(*window);
}

static void begin(EventReader<WindowEvent> events)
{
    // Pass window events to ImGui.
    // TODO: handleEvent returns a bool indicating if the event was handled or not.
    //       This will be used to stop propagating the event to other systems when
    //       the mouse is over an ImGui window.
    //       Not sure how we will propagate that information to other systems yet.
    for (auto event : events)
    {
        ui::handleEvent(event);
    }

    ui::beginFrame();
}

static void end()
{
    ui::endFrame();
}

void cubos::engine::imguiPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.startupTag("cubos.imgui.init").after("cubos.window.init");
    cubos.tag("cubos.imgui.begin").after("cubos.window.poll");
    cubos.tag("cubos.imgui.end").before("cubos.window.render").after("cubos.imgui.begin");
    cubos.tag("cubos.imgui").after("cubos.imgui.begin").before("cubos.imgui.end");

    cubos.startupSystem(init).tagged("cubos.imgui.init");
    cubos.system(begin).tagged("cubos.imgui.begin");
    cubos.system(end).tagged("cubos.imgui.end");
}
