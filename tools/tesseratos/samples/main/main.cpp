#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <tesseratos/plugin.hpp>

using cubos::engine::ActiveCameras;
using cubos::engine::AnyAsset;
using cubos::engine::Asset;
using cubos::engine::Assets;
using cubos::engine::Camera;
using cubos::engine::Commands;
using cubos::engine::Cubos;
using cubos::engine::Input;
using cubos::engine::InputBindings;
using cubos::engine::LocalToWorld;
using cubos::engine::Position;
using cubos::engine::Rotation;
using cubos::engine::Settings;

static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842e5");

int main(int argc, char** argv)

{
    Cubos cubos{argc, argv};
    cubos.addPlugin(tesseratos::plugin);
    cubos.addPlugin(cubos::engine::inputPlugin);

    cubos.startupSystem("configure Assets plugin").tagged(cubos::engine::settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
        settings.setBool("assets.io.readOnly", false);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(cubos::engine::assetsTag)
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
        });

    cubos.run();
}
