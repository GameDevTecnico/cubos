#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/audio/pause.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioPause)
{
    return core::ecs::TypeBuilder<AudioPause>("cubos::engine::AudioPause").build();
}
