#include <cubos/core/io/window.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/screen_picker/plugin.hpp>
#include <cubos/engine/screen_picker/screen_picker.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::ResizeEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

using namespace cubos::engine;

static void processResize(ScreenPicker& screenPicker, EventReader<WindowEvent> windowEvent)
{
    for (const auto& event : windowEvent)
    {
        if (std::holds_alternative<ResizeEvent>(event))
        {
            screenPicker.resizeTexture(std::get<ResizeEvent>(event).size);
        }
    }
}

static void clearTexture(ScreenPicker& screenPicker)
{
    screenPicker.clearTexture();
}

static void initScreenPickerSystem(ScreenPicker& screenPicker, const Window& window)
{
    screenPicker.init(&window->renderDevice(), window->framebufferSize());
}

void cubos::engine::screenPickerPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.addResource<ScreenPicker>();

    cubos.startupSystem(initScreenPickerSystem).tagged("cubos.screenPicker.init").after("cubos.window.init");

    cubos.system(clearTexture).tagged("cubos.screenPicker.clear");
    cubos.system(processResize)
        .tagged("cubos.screenPicker.resize")
        .after("cubos.window.poll")
        .before("cubos.screenPicker.clear");
}
