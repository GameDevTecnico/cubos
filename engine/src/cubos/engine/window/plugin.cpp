#include <cubos/core/settings.hpp>

#include <cubos/engine/window/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::EventWriter;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::io::openWindow;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;
using namespace cubos::engine;

static void init(Write<Window> window, Write<ShouldQuit> quit, Read<Settings> settings)
{
    quit->value = false;
    *window = openWindow(settings->getString("window.title", "Cubos"),
                         {settings->getInteger("window.width", 800), settings->getInteger("window.height", 600)});
}

static void poll(Read<Window> window, Write<ShouldQuit> quit, EventWriter<WindowEvent> events)
{
    // Send window events to other systems.
    while (auto event = (*window)->pollEvent())
    {
        events.push(event.value());
    }

    if ((*window)->shouldClose())
    {
        quit->value = true;
    }
}

static void render(Read<Window> window)
{
    (*window)->swapBuffers();
}

void cubos::engine::windowPlugin(Cubos& cubos)
{
    cubos.addResource<Window>();
    cubos.addEvent<WindowEvent>();

    cubos.startupTag("cubos.window.init").afterTag("cubos.settings");
    cubos.tag("cubos.window.poll").beforeTag("cubos.window.render");

    cubos.startupSystem(init).tagged("cubos.window.init");
    cubos.system(poll).tagged("cubos.window.poll");
    cubos.system(render).tagged("cubos.window.render");
}
