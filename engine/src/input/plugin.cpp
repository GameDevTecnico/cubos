#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::inputUpdateTag);

using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::inputPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(windowPlugin);

    cubos.resource<Input>();

    cubos.tag(inputUpdateTag);

    cubos.startupSystem("setup InputBindings asset bridge").tagged(assetsBridgeTag).call([](Assets& assets) {
        assets.registerBridge(".bind", std::make_unique<JSONBridge<InputBindings>>());
    });

    cubos.system("handle WindowEvents for Input")
        .tagged(inputUpdateTag)
        .after(windowPollTag)
        .call([](const Window& window, Input& input, EventReader<WindowEvent> events) {
            input.updateMouse();

            for (auto event : events)
            {
                std::visit([window, &input](auto e) { input.handle(window, e); }, event);
            }

            input.pollGamepads(window);
        });
}
