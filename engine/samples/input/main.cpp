#include <cubos/core/settings.hpp>

#include <cubos/engine/input/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

int main()
{
    auto cubos = Cubos();
    cubos.addPlugin(inputPlugin);
    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.run();
    return 0;
}
