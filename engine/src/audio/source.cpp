#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/audio/source.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioSource)
{
    return core::ecs::TypeBuilder<AudioSource>("cubos::engine::AudioSource").build();
}