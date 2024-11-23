#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;

/// [Run]
int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.plugin(settingsPlugin);

    cubos.startupSystem("print setting value").after(settingsTag).call([](Settings& settings) {
        CUBOS_INFO("{}", settings.getString("greeting", "Hello!"));
        settings.save();
    });

    cubos.startupSystem("print number setting value").after(settingsTag).call([](Settings& settings) {
        CUBOS_INFO("{}", settings.getInteger("number", 1));
        CUBOS_INFO("{}", settings.getDouble("double", 1e-07));
        CUBOS_INFO("{}", settings.getBool("boolean", false));
        CUBOS_INFO("{}", settings.getString("emptyString", ""));
        settings.save();
    });

    cubos.run();
}
/// [Run]
