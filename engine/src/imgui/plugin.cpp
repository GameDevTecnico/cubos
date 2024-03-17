#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "imgui.hpp"

CUBOS_DEFINE_TAG(cubos::engine::ImguiInitTag);
CUBOS_DEFINE_TAG(cubos::engine::ImguiBeginTag);
CUBOS_DEFINE_TAG(cubos::engine::ImguiEndTag);
CUBOS_DEFINE_TAG(cubos::engine::ImguiTag);

using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::imguiPlugin(Cubos& cubos)
{
    cubos.addPlugin(windowPlugin);
    cubos.addPlugin(rendererPlugin);

    cubos.addResource<DataInspector>();

    cubos.startupTag(ImguiInitTag).after(WindowPollTag);
    cubos.tag(ImguiBeginTag).after(RendererDrawTag);
    cubos.tag(ImguiEndTag).before(WindowRenderTag).after(ImguiBeginTag);
    cubos.tag(ImguiTag).after(ImguiBeginTag).before(ImguiEndTag);

    cubos.startupSystem("initialize ImGui").tagged(ImguiInitTag).call([](const Window& window, Settings& settings) {
        float dpiScale = static_cast<float>(settings.getDouble("imgui.scale", window->contentScale()));
        imguiInitialize(window, dpiScale);
    });

    cubos.system("begin ImGui frame").tagged(ImguiBeginTag).call([](EventReader<WindowEvent> events) {
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

    cubos.system("end ImGui frame").tagged(ImguiEndTag).call([]() { imguiEndFrame(); });
}
