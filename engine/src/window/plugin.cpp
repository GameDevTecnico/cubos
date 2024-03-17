#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::WindowInitTag);
CUBOS_DEFINE_TAG(cubos::engine::WindowPollTag);
CUBOS_DEFINE_TAG(cubos::engine::WindowRenderTag);

using cubos::core::io::openWindow;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::windowPlugin(Cubos& cubos)
{

    cubos.addPlugin(settingsPlugin);

    cubos.addResource<Window>();
    cubos.addEvent<WindowEvent>();

    cubos.startupTag(WindowInitTag).after(SettingsTag);
    cubos.tag(WindowPollTag).before(WindowRenderTag);

    cubos.startupSystem("open Window")
        .tagged(WindowInitTag)
        .call([](Window& window, ShouldQuit& quit, Settings& settings) {
            quit.value = false;
            window = openWindow(settings.getString("window.title", "CUBOS."),
                                {settings.getInteger("window.width", 800), settings.getInteger("window.height", 600)});
        });

    cubos.system("poll Window events")
        .tagged(WindowPollTag)
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

    cubos.system("swap Window buffers").tagged(WindowRenderTag).call([](const Window& window) {
        window->swapBuffers();
    });
}
