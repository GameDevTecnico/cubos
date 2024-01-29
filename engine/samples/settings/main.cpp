#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;

/// [System]
static void checkSettings(Settings& settings)
{
    CUBOS_INFO("{}", settings.getString("greeting", "Hello!"));
}
/// [System]

/// [Run]
int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(settingsPlugin);
    cubos.startupSystem(checkSettings).after("cubos.settings");
    cubos.run();
}
/// [Run]
