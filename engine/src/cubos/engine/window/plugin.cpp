#include <cubos/core/settings.hpp>

#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core;

static void init(io::Window& window, ShouldQuit& quit, const Settings& settings)
{
    quit.value = false;
    window = io::openWindow(settings.getString("window.title", "Cubos"),
                            {settings.getInteger("window.width", 800), settings.getInteger("window.height", 600)});
}

static void poll(const io::Window& window, ShouldQuit& quit, ecs::EventWriter<io::WindowEvent> events)
{
    // Send window events to other systems.
    while (auto event = window->pollEvent())
    {
        events.push(event.value());
    }

    if (window->shouldClose())
    {
        quit.value = true;
    }
}

static void render(const io::Window& window)
{
    window->swapBuffers();
}

void cubos::engine::plugins::windowPlugin(Cubos& cubos)
{
    cubos.addResource<io::Window>();
    cubos.addEvent<io::WindowEvent>();

    cubos.startupTag("cubos.window.init").afterTag("cubos.settings");
    cubos.tag("cubos.window.poll").beforeTag("cubos.window.render");

    cubos.startupSystem(init).tagged("cubos.window.init");
    cubos.system(poll).tagged("cubos.window.poll");
    cubos.system(render).tagged("cubos.window.render");
}
