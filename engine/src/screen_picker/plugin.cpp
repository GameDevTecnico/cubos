#include <cubos/core/io/window.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/screen_picker/plugin.hpp>
#include <cubos/engine/screen_picker/screen_picker.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::screenPickerInitTag);
CUBOS_DEFINE_TAG(cubos::engine::screenPickerResizeTag);
CUBOS_DEFINE_TAG(cubos::engine::screenPickerDrawTag);

using cubos::core::io::ResizeEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::screenPickerPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);

    cubos.resource<ScreenPicker>();

    cubos.startupTag(screenPickerInitTag);

    cubos.tag(screenPickerResizeTag);
    cubos.tag(screenPickerDrawTag);

    cubos.startupSystem("initialize ScreenPicker")
        .tagged(screenPickerInitTag)
        .after(windowInitTag)
        .call([](ScreenPicker& screenPicker, const Window& window) {
            screenPicker.init(&window->renderDevice(), window->framebufferSize());
        });

    cubos.system("update ScreenPicker on resize")
        .tagged(screenPickerResizeTag)
        .after(windowPollTag)
        .call([](ScreenPicker& screenPicker, EventReader<WindowEvent> windowEvent) {
            for (const auto& event : windowEvent)
            {
                if (std::holds_alternative<ResizeEvent>(event))
                {
                    screenPicker.resizeTexture(std::get<ResizeEvent>(event).size);
                }
            }
        });

    cubos.system("clear ScreenPicker texture")
        .after(screenPickerResizeTag)
        .before(screenPickerDrawTag)
        .call([](ScreenPicker& screenPicker) { screenPicker.clearTexture(); });
}
