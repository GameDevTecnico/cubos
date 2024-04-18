#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/hdr/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("05db7643-a8b8-49cd-8c16-0992136cfacf");

int main()
{
    Cubos cubos{};

    /// [Adding the plugins]
    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(shaderPlugin);
    cubos.plugin(renderTargetPlugin);
    cubos.plugin(hdrPlugin);
    cubos.plugin(toneMappingPlugin);
    /// [Adding the plugins]

    cubos.plugin(scenePlugin);

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("spawn the scene").tagged(assetsTag).call([](Commands commands, const Assets& assets) {
        commands.spawn(assets.read(SceneAsset)->blueprint);
    });

    cubos.run();
}
