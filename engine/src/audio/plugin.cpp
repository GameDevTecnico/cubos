#include <cubos/core/al/audio_context.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/audio/bridge.hpp>
#include <cubos/engine/audio/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::audioInitTag);

using cubos::core::al::AudioContext;

void cubos::engine::audioPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);

    cubos.component<AudioSource>();
    cubos.component<AudioListener>();

    cubos.startupTag(audioInitTag).after(settingsTag);

    cubos.startupSystem("initalize Audio plugin")
        .tagged(audioInitTag)
        .tagged(assetsBridgeTag)
        .call([](Assets& assets, Settings& settings) {
            auto audioContext = AudioContext::create();

            unsigned int listenerCount = static_cast<unsigned int>(
                settings.getInteger("audio.listener.count", static_cast<int>(MA_ENGINE_MAX_LISTENERS)));

            auto audioDevice = audioContext->createDevice(listenerCount);

            auto bridge = std::make_shared<AudioBridge>(audioContext);
            assets.registerBridge(".wav", bridge);
            assets.registerBridge(".mp3", bridge);
            assets.registerBridge(".flac", bridge);
            assets.registerBridge(".ogg", bridge);
        });
}