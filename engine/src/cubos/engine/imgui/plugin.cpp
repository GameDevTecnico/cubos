#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "imgui.hpp"

using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::imguiPlugin(Cubos& cubos)
{
    cubos.addPlugin(windowPlugin);
    cubos.addPlugin(rendererPlugin);

    cubos.addResource<DataInspector>();

    cubos.startupTag("cubos.imgui.init").after("cubos.window.init");
    cubos.tag("cubos.imgui.begin").after("cubos.renderer.draw");
    cubos.tag("cubos.imgui.end").before("cubos.window.render").after("cubos.imgui.begin");
    cubos.tag("cubos.imgui").after("cubos.imgui.begin").before("cubos.imgui.end");

    cubos.startupSystem("initialize ImGui")
        .tagged("cubos.imgui.init")
        .call([](const Window& window, Settings& settings) {
            float dpiScale = static_cast<float>(settings.getDouble("imgui.scale", window->contentScale()));
            imguiInitialize(window, dpiScale);
        });

    cubos.system("begin ImGui frame").tagged("cubos.imgui.begin").call([](EventReader<WindowEvent> events) {
        // Pass window events to ImGui.
        // TODO: handleEvent returns a bool indicating if the event was handled or not.
        //       This will be used to stop propagating the event to other systems when
        //       the mouse is over an ImGui window.
        //       Not sure how we will propagate that information to other systems yet.
        for (auto event : events)
        {
            imguiHandleEvent(event);
        }

        imguiBeginFrame();
    });

    cubos.system("end ImGui frame").tagged("cubos.imgui.end").call([]() { imguiEndFrame(); });
}
