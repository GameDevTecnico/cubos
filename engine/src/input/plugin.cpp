#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::InputUpdateTag);

using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

void cubos::engine::inputPlugin(Cubos& cubos)
{

    cubos.addPlugin(cubos::engine::assetsPlugin);
    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.addResource<Input>();

    cubos.startupSystem("setup InputBindings asset bridge").tagged(AssetsBridgeTag).call([](Assets& assets) {
        assets.registerBridge(".bind", std::make_unique<JSONBridge<InputBindings>>());
    });

    cubos.system("handle WindowEvents for Input")
        .tagged(InputUpdateTag)
        .after(WindowPollTag)
        .call([](const Window& window, Input& input, EventReader<WindowEvent> events) {
            input.updateMouse();

            for (auto event : events)
            {
                std::visit([window, &input](auto e) { input.handle(window, e); }, event);
            }

            input.pollGamepads(window);
        });
}
