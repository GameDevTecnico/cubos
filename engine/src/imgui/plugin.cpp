#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "imgui.hpp"

CUBOS_DEFINE_TAG(cubos::engine::imguiInitTag);
CUBOS_DEFINE_TAG(cubos::engine::imguiBeginTag);
CUBOS_DEFINE_TAG(cubos::engine::imguiEndTag);
CUBOS_DEFINE_TAG(cubos::engine::imguiTag);

using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::imguiPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(renderTargetPlugin);

    cubos.resource<DataInspector>();

    cubos.startupTag(imguiInitTag).after(windowInitTag);

    cubos.tag(imguiBeginTag).after(windowPollTag);
    cubos.tag(imguiEndTag).before(windowRenderTag).after(imguiBeginTag);
    cubos.tag(imguiTag).after(imguiBeginTag).before(imguiEndTag);

    cubos.startupSystem("initialize ImGui").tagged(imguiInitTag).call([](const Window& window, Settings& settings) {
        float dpiScale = static_cast<float>(settings.getDouble("imgui.scale", window->contentScale()));
        imguiInitialize(window, dpiScale);
    });

    cubos.system("begin ImGui frame").tagged(imguiBeginTag).call([](EventReader<WindowEvent> events) {
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

    cubos.system("end ImGui frame").tagged(imguiEndTag).call([](const Window& window, Query<RenderTarget&> targets) {
        bool cleared = false;
        for (auto [target] : targets)
        {
            if (target.framebuffer == nullptr)
            {
                cleared = target.cleared;
                target.cleared = true;
                break;
            }
        }

        if (!cleared)
        {
            window->renderDevice().setFramebuffer(nullptr);
            window->renderDevice().setScissor(0, 0, static_cast<int>(window->framebufferSize().x),
                                              static_cast<int>(window->framebufferSize().y));
            window->renderDevice().clearColor(0.0F, 0.0F, 0.0F, 0.0F);
        }

        imguiEndFrame();
    });
}
