#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/plugin.hpp>

using cubos::core::Settings;
using cubos::core::data::Debug;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static const Asset<InputBindings> bindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void init(Read<Assets> assets, Write<Input> input)
{
    auto bindings = assets->read<InputBindings>(bindingsAsset);
    input->bind(0, *bindings);
    CUBOS_INFO("{}", Debug(input->bindings().at(0)));
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
