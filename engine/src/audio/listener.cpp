#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/audio/listener.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioListener)
{
    return core::ecs::TypeBuilder<AudioListener>("cubos::engine::AudioListener")
        .withField("active", &AudioListener::active)
        .build();
}