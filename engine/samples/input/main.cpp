#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static const Asset<Bindings> bindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void init(Read<Assets> assets, Write<Input> input)
{
    auto bindings = assets->read<Bindings>(bindingsAsset);
    input->setBindings(0, *bindings);
    CUBOS_INFO("{}", input->getBindings().at(0).toString());
}

int main()
{
    auto cubos = Cubos();
    cubos.addPlugin(inputPlugin);
    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.startupSystem(init).tagged("cubos.assets");
    cubos.run();
    return 0;
}
