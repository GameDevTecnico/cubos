#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;

/// [Run]
int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.addPlugin(settingsPlugin);

    cubos.startupSystem("print setting value").after(settingsTag).call([](Settings& settings) {
        CUBOS_INFO("{}", settings.getString("greeting", "Hello!"));
    });

    cubos.run();
}
/// [Run]
