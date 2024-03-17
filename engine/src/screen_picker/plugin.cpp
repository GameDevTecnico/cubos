#include <cubos/core/io/window.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/screen_picker/plugin.hpp>
#include <cubos/engine/screen_picker/screen_picker.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::screenPickerInitTag);
CUBOS_DEFINE_TAG(cubos::engine::screenPickerClearTag);
CUBOS_DEFINE_TAG(cubos::engine::screenPickerResizeTag);

using cubos::core::io::ResizeEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::screenPickerPlugin(Cubos& cubos)
{

    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.addResource<ScreenPicker>();

    cubos.startupSystem("initialize ScreenPicker")
        .tagged(screenPickerInitTag)
        .after(windowInitTag)
        .call([](ScreenPicker& screenPicker, const Window& window) {
            screenPicker.init(&window->renderDevice(), window->framebufferSize());
        });

    cubos.system("clear ScreenPicker texture").tagged(screenPickerClearTag).call([](ScreenPicker& screenPicker) {
        screenPicker.clearTexture();
    });

    cubos.system("update ScreenPicker on resize")
        .tagged(screenPickerResizeTag)
        .after(windowPollTag)
        .before(screenPickerClearTag)
        .call([](ScreenPicker& screenPicker, EventReader<WindowEvent> windowEvent) {
            for (const auto& event : windowEvent)
            {
                if (std::holds_alternative<ResizeEvent>(event))
                {
                    screenPicker.resizeTexture(std::get<ResizeEvent>(event).size);
                }
            }
        });
}
