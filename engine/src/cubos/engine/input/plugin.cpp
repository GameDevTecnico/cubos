#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::EventReader;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;
using namespace cubos::engine;

static void bridge(Write<Assets> assets)
{
    assets->registerBridge(".bind", std::make_unique<JSONBridge<InputBindings>>());
}

static void update(Read<Window> window, Write<Input> input, EventReader<WindowEvent> events)
{
    for (auto event : events)
    {
        std::visit([window, &input](auto e) { input->handle(*window, e); }, event);
    }

    input->pollGamepads(*window);
}

void cubos::engine::inputPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.addResource<Input>();

    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
    cubos.system(update).tagged("cubos.input.update").after("cubos.window.poll");
}
