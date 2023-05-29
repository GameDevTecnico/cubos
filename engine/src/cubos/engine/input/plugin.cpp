#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::EventReader;
using cubos::core::ecs::Write;
using cubos::core::io::WindowEvent;
using namespace cubos::engine;

static void bridge(Write<Assets> assets)
{
    assets->registerBridge(".bind", std::make_unique<JSONBridge<InputBindings>>());
}

static void update(Write<Input> input, EventReader<WindowEvent> events)
{
    for (auto event : events)
    {
        std::visit([&input](auto e) { input->handle(e); }, event);
    }
}

void cubos::engine::inputPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.addResource<Input>();

    cubos.tag("cubos.input.update").afterTag("cubos.window.poll");

    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
    cubos.system(update).tagged("cubos.input.update");
}
