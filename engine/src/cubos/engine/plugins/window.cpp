#include <cubos/engine/plugins/window.hpp>
#include <cubos/core/settings.hpp>

using namespace cubos::core;

static void startup(io::Window& window, ShouldQuit& quit, const Settings& settings)
{
    quit.value = false;
    window = io::openWindow(settings.getString("window.title", "Cubos"),
                            {settings.getInteger("window.width", 800), settings.getInteger("window.height", 600)});
}

static void pollSystem(const io::Window& window, ShouldQuit& quit, ecs::EventWriter<io::WindowEvent> events)
{
    // Send window events to other systems.
    while (auto event = window->pollEvent())
        events.push(event.value());

    if (window->shouldClose())
    {
        quit.value = true;
    }
}

static void swapBuffersSystem(const io::Window& window)
{
    window->swapBuffers();
}

void cubos::engine::plugins::windowPlugin(Cubos& cubos)
{
    cubos.addResource<io::Window>();
    cubos.addEvent<io::WindowEvent>();

    cubos.startupSystem(startup).tagged("OpenWindow");

    cubos.system(pollSystem).tagged("Poll");

    cubos.system(swapBuffersSystem).tagged("SwapBuffers");
}
