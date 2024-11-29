#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/audio/play.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioPlay)
{
    return core::ecs::TypeBuilder<AudioPlay>("cubos::engine::AudioPlay").build();
}
