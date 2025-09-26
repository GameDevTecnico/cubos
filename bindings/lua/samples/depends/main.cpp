#include <cubos/bindings/lua/plugin.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;
using namespace cubos::bindings::lua;

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.plugin(settingsPlugin);
    cubos.plugin(luaBindingsPlugin);
    cubos.plugin(transformPlugin);

    cubos.startupSystem("configure Assets").before(settingsTag).call([](Settings& settings) {
        settings.setString("scripts.lua.app.osPath", APP_SCRIPTS_PATH);
    });

    cubos.run();
    return 0;
}
