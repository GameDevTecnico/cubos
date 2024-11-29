#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/audio/pause.hpp>
#include <cubos/engine/audio/play.hpp>
#include <cubos/engine/audio/plugin.hpp>
#include <cubos/engine/audio/stop.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

/// [Get handles to assets]
static const Asset<Audio> AudioAssetMP3 = AnyAsset("059c16e7-a439-44c7-9bdc-7f133dba0c80");
static const Asset<Audio> AudioAssetWAV = AnyAsset("3f93e774-888c-4ead-8819-67fc7e873df0");
static const Asset<Audio> AudioAssetFLAC = AnyAsset("ccf3b646-1307-430d-bf3e-a23e06430043");
static const Asset<InputBindings> BindingsAsset = AnyAsset("6709a9ce-8651-4295-bdf2-848b052ce1f7");
static const std::array<Asset<Audio>, 3> AudioAssets = {AudioAssetMP3, AudioAssetWAV, AudioAssetFLAC};
/// [Get handles to assets]

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    size_t currAsset = 0;

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(fixedStepPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(collisionsPlugin);
    cubos.plugin(physicsPlugin);
    cubos.plugin(inputPlugin);
    cubos.plugin(audioPlugin);

    cubos.startupSystem("configure Assets").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(assetsTag)
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
        });

    cubos.startupSystem("create a camera").call([](Commands cmds) {
        cmds.create()
            .add(Position{{0.0F, 0.0F, 0.0F}})
            .add(Rotation::lookingAt({-1.0F, -1.0F, -1.0F}, glm::vec3{0.0F, 1.0F, 0.0F}))
            .add(AudioListener{true});
    });

    cubos.startupSystem("create an audio source").after(audioStateInitTag).call([](Commands cmds) {
        cmds.create()
            .add(Position{{0.0F, 0.0F, 0.0F}})
            .add(Velocity{.vec = {1.0F, 1.0F, 1.0F}})
            .add(Rotation::lookingAt({-1.0F, -1.0F, -1.0F}, glm::vec3{0.0F, 1.0F, 0.0F}))
            .add(AudioSource{});
    });

    cubos.system("play audio").call([&currAsset](Input& input, Commands cmds, Query<Entity, AudioSource&> query) {
        for (auto [ent, src] : query)
        {
            if (src.sound.isNull())
            {
                src.sound = AudioAssets[0];
                cmds.add(ent, AudioPlay{});
            }

            if (input.justPressed("skip"))
            {
                CUBOS_INFO("SKIP: {}", src.sound.getIdString());
                cmds.add(ent, AudioStop{});
                src.sound = AudioAssets[++currAsset % 3];
                CUBOS_INFO("PLAYING: {}", src.sound.getIdString());
                cmds.add(ent, AudioPlay{});
            }
            if (input.justPressed("play_pause"))
            {
                if (src.playing)
                {
                    cmds.add(ent, AudioPause{});
                    CUBOS_INFO("PAUSING: {}", src.sound.getIdString());
                }
                else
                {
                    cmds.add(ent, AudioPlay{});
                    CUBOS_INFO("PLAYING: {}", src.sound.getIdString());
                }
            }
            if (input.justPressed("stop"))
            {
                cmds.add(ent, AudioStop{});
                CUBOS_INFO("STOPPING: {}", src.sound.getIdString());
            }
        }
    });

    cubos.run();
}
