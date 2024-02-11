#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
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

    cubos.startupSystem("configure Assets plugin").tagged("cubos.settings").call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
        settings.setBool("assets.io.readOnly", false);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged("cubos.assets")
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
        });

    cubos.startupSystem("create a Camera").call([](ActiveCameras& camera, Commands cmds) {
        camera.entities[0] =
            cmds.create()
                .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 1000.0F})
                .add(LocalToWorld{})
                .add(Position{{10.0F, 10.0F, 0.0F}})
                .add(Rotation{glm::quatLookAt(glm::vec3{-1.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
                .entity();
        cmds.create().add(LocalToWorld{}).add(Position{{3.0F, 3.0F, 0.0F}}).entity();
    });

    cubos.run();
}
