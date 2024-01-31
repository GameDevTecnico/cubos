#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::openWindow;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::windowPlugin(Cubos& cubos)
{
    cubos.addPlugin(settingsPlugin);

    cubos.addResource<Window>();
    cubos.addEvent<WindowEvent>();

    cubos.startupTag("cubos.window.init").after("cubos.settings");
    cubos.tag("cubos.window.poll").before("cubos.window.render");

    cubos.startupSystem("open Window")
        .tagged("cubos.window.init")
        .call([](Window& window, ShouldQuit& quit, Settings& settings) {
            quit.value = false;
            window = openWindow(settings.getString("window.title", "CUBOS."),
                                {settings.getInteger("window.width", 800), settings.getInteger("window.height", 600)});
        });

    cubos.system("poll Window events")
        .tagged("cubos.window.poll")
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

    cubos.system("swap Window buffers").tagged("cubos.window.render").call([](const Window& window) {
        window->swapBuffers();
    });
}
