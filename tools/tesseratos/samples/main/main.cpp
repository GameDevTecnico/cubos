#include <cubos/engine/settings/settings.hpp>

#include <tesseratos/plugin.hpp>

using cubos::core::ecs::Write;

using cubos::engine::Cubos;
using cubos::engine::Settings;

static void settingsSystem(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    settings->setBool("assets.io.readOnly", false);
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(tesseratos::plugin);
    cubos.startupSystem(settingsSystem).tagged("cubos.settings");
    cubos.run();
}
