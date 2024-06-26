#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::windowInitTag);
CUBOS_DEFINE_TAG(cubos::engine::windowPollTag);
CUBOS_DEFINE_TAG(cubos::engine::windowRenderTag);

using cubos::core::io::openWindow;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

template class cubos::core::ecs::EventPipe<WindowEvent>;

void cubos::engine::windowPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);

    cubos.uninitResource<Window>();
    cubos.event<WindowEvent>();

    cubos.startupTag(windowInitTag).after(settingsTag);
    cubos.tag(windowPollTag);
    cubos.tag(windowRenderTag).after(windowPollTag);

    cubos.startupSystem("open Window")
        .tagged(windowInitTag)
        .call([](Commands cmds, ShouldQuit& quit, Settings& settings) {
            quit.value = false;
            cmds.insertResource(
                openWindow(settings.getString("window.title", "Cubos"),
                           {settings.getInteger("window.width", 800), settings.getInteger("window.height", 600)},
                           settings.getBool("window.vSync", true)));
        });

    cubos.system("poll Window events")
        .tagged(windowPollTag)
        .call([](const Window& window, ShouldQuit& quit, EventWriter<WindowEvent> events) {
            // Send window events to other systems.
            while (auto event = window->pollEvent())
            {
                events.push(event.value());
            }

            if (window->shouldClose())
            {
                quit.value = true;
            }
        });

    cubos.system("swap Window buffers").tagged(windowRenderTag).call([](const Window& window) {
        window->swapBuffers();
    });
}
