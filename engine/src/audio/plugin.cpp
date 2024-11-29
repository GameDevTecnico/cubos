#include <cubos/core/al/audio_context.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/audio/bridge.hpp>
#include <cubos/engine/audio/plugin.hpp>
#include <cubos/engine/physics/components/velocity.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "listener_impl.hpp"
#include "source_impl.hpp"

CUBOS_DEFINE_TAG(cubos::engine::audioInitTag);
CUBOS_DEFINE_TAG(cubos::engine::audioStateInitTag);

using cubos::core::al::AudioContext;

using namespace cubos::core::ecs;

using namespace cubos::engine;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        std::queue<std::size_t> freeListeners{};
        cubos::core::al::AudioDevice audioDevice;
        std::shared_ptr<cubos::core::al::AudioContext> audioContext;
    };
} // namespace

void cubos::engine::audioPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);

    cubos.component<AudioSource>();
    cubos.component<AudioListener>();
    cubos.component<AudioSourceImpl>();
    cubos.component<AudioListenerImpl>();
    cubos.component<AudioPlay>();
    cubos.component<AudioPause>();
    cubos.component<AudioStop>();

    cubos.uninitResource<State>();

    cubos.startupTag(audioInitTag).after(settingsTag);
    cubos.startupTag(audioStateInitTag).after(audioInitTag);

    cubos.startupSystem("initalize Audio plugin's state")
        .tagged(audioInitTag)
        .tagged(assetsBridgeTag)
        .call([](Commands cmds) { cmds.emplaceResource<State>(); });

    cubos.startupSystem("initialize Audio plugin")
        .tagged(audioStateInitTag)
        .after(settingsTag)
        .call([](Assets& assets, Settings& settings, State& state) {
            state.audioContext = AudioContext::create();
            unsigned int listenerCount = static_cast<unsigned int>(settings.getInteger(
                "audio.listener.count", static_cast<int>(state.audioContext->getMaxListenerCount())));

            state.audioDevice = state.audioContext->createDevice(listenerCount);

            for (unsigned int i = 0; i < listenerCount; ++i)
            {
                state.freeListeners.push(i);
            }

            auto bridge = std::make_shared<AudioBridge>(state.audioContext);
            assets.registerBridge(".wav", bridge);
            assets.registerBridge(".mp3", bridge);
            assets.registerBridge(".flac", bridge);
            assets.registerBridge(".ogg", bridge);
        });

    cubos.observer("register AudioListener").onAdd<AudioListener>().call([](Commands cmds, Query<Entity> query) {
        for (auto [ent] : query)
        {
            cmds.add(ent, AudioListenerImpl{});
        }
    });

    cubos.observer("unregister AudioListener").onRemove<AudioListener>().call([](Commands cmds, Query<Entity> query) {
        for (auto [ent] : query)
        {
            cmds.remove<AudioListenerImpl>(ent);
        }
    });

    cubos.observer("register AudioSource")
        .onAdd<AudioSource>()
        .call([](Commands cmds, State& state, Query<Entity, const AudioSource&> query) {
            for (auto [ent, src] : query)
            {
                cmds.add(ent, AudioSourceImpl{state.audioDevice->createSource(), src.sound});
            }
        });

    cubos.observer("unregister AudioSource").onRemove<AudioSource>().call([](Commands cmds, Query<Entity> query) {
        for (auto [ent] : query)
        {
            cmds.remove<AudioSourceImpl>(ent);
        }
    });

    cubos.observer("handle Audio play")
        .onAdd<AudioPlay>()
        .call([](Assets& assets, Commands cmds, Query<Entity, AudioSource&, AudioSourceImpl&, const AudioPlay&> query) {
            for (auto [ent, src, impl, _] : query)
            {
                if (src.sound.isNull())
                {
                    CUBOS_FAIL("AudioPlay component added to entity without a sound asset");
                    return;
                }

                if (!src.sound.isStrong())
                {
                    impl.sound = src.sound;
                    src.sound = assets.load(src.sound);
                    auto sound = assets.read<Audio>(src.sound);

                    impl.source->setBuffer(sound->buffer);
                }

                // Pass the atributes of the component to core class
                impl.source->setGain(src.gain);
                impl.source->setPitch(src.pitch);
                impl.source->setLooping(src.looping);

                impl.source->setCone(src.innerConeAngle, src.outerConeAngle, src.outerConeGain);
                impl.source->setMinDistance(src.minDistance);
                impl.source->setMaxDistance(src.maxDistance);

                impl.source->play();
                src.playing = true;

                cmds.remove<AudioPlay>(ent);
            }
        });

    cubos.observer("handle Audio pause")
        .onAdd<AudioPause>()
        .call([](Commands cmds, State& state,
                 Query<Entity, AudioSource&, const AudioSourceImpl&, const AudioPause&> query) {
            (void)state;
            for (auto [ent, src, impl, _] : query)
            {
                cmds.remove<AudioPause>(ent);

                impl.source->pause();
                src.playing = false;
            }
        });

    cubos.observer("handle Audio stop")
        .onAdd<AudioStop>()
        .call([](Commands cmds, State& state,
                 Query<Entity, AudioSource&, const AudioSourceImpl&, const AudioStop&> query) {
            (void)state;
            for (auto [ent, src, impl, _] : query)
            {
                cmds.remove<AudioStop>(ent);

                impl.source->stop();
                src.playing = false;
            }
        });

    cubos.system("update AudioListeners and AudioSources")
        .call([](const State& state,
                 Query<const LocalToWorld&, Opt<const Velocity&>, const AudioListener&, const AudioListenerImpl&>
                     listeners,
                 Query<const LocalToWorld&, Opt<const Velocity&>, const AudioSourceImpl&> sources) {
            for (auto [ltw, vel, listener, impl] : listeners)
            {
                if (listener.active && impl.index.contains())
                {
                    state.audioDevice->listener(impl.index.value())->setPosition(ltw.worldPosition());
                    state.audioDevice->listener(impl.index.value())->setOrientation(ltw.forward(), ltw.up());

                    if (vel.contains())
                    {
                        state.audioDevice->listener(impl.index.value())->setVelocity(vel->vec);
                    }
                }
            }

            for (auto [ltw, vel, impl] : sources)
            {
                impl.source->setPosition(ltw.worldPosition());

                if (vel.contains())
                {
                    impl.source->setVelocity(vel->vec);
                }
            }
        });

    cubos.system("manage AudioListener active state")
        .call([](State& state, Query<AudioListener&, AudioListenerImpl&> query) {
            for (auto [listener, impl] : query)
            {
                if (listener.active && !impl.index.contains())
                {
                    if (!state.freeListeners.empty())
                    {
                        impl.index = state.freeListeners.front();
                        state.freeListeners.pop();
                        return;
                    }

                    CUBOS_WARN("You are trying to activate more listeners but none are free, current max listener "
                               "count: {}",
                               state.audioContext->getMaxListenerCount());
                }
                else if (!listener.active &&
                         impl.index.contains()) // if its deactivated and still has an assigned index
                {
                    state.freeListeners.push(impl.index.value());
                    impl.index.reset();
                }
            }
        });
}
