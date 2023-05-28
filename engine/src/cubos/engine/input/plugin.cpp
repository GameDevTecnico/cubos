/// @file
/// @brief Input plugin entrypoint, registers the input bindings bridge.

#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/plugin.hpp>

using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static const Asset<Bindings> bindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");

static void bridge(Write<Assets> assets)
{
    assets->registerBridge(".bind", std::make_unique<JSONBridge<Bindings>>());
}

static void init(Read<Assets> assets)
{
    auto bindings = assets->read<Bindings>(bindingsAsset);
    CUBOS_INFO("{}", bindings->toString());
}

void cubos::engine::inputPlugin(Cubos& cubos)
{
    cubos.addPlugin(assetsPlugin);
    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
    cubos.startupSystem(init).tagged("cubos.assets");
}