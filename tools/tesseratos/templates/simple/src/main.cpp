#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> MainSceneAsset = AnyAsset("ba173db6-4e44-45bc-9cef-c4a25bba8823");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);

    // TODO: add your own game plugins here!

    cubos.startupSystem("set the assets directory path").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets) { cmds.spawn(assets.read(MainSceneAsset)->blueprint); });

    cubos.run();
}
