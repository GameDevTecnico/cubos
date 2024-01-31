#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::inputPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.addResource<Input>();

    cubos.startupSystem("setup InputBindings asset bridge").tagged("cubos.assets.bridge").call([](Assets& assets) {
        assets.registerBridge(".bind", std::make_unique<JSONBridge<InputBindings>>());
    });

    cubos.system("handle WindowEvents for Input")
        .tagged("cubos.input.update")
        .after("cubos.window.poll")
        .call([](const Window& window, Input& input, EventReader<WindowEvent> events) {
            input.updateMouse();

            for (auto event : events)
            {
                std::visit([window, &input](auto e) { input.handle(window, e); }, event);
            }

            input.pollGamepads(window);
        });
}
