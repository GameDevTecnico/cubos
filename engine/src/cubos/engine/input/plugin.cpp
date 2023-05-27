/// @file
/// @brief Input plugin entrypoint, registers the input bindings bridge.

#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/plugin.hpp>

using cubos::core::ecs::Write;
using namespace cubos::engine;

static void bridge(Write<Assets> assets)
{
    assets->registerBridge(".bind", std::make_unique<JSONBridge<Bindings>>());
}

void cubos::engine::inputPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);
    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
}