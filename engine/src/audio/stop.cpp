#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/audio/stop.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioStop)
{
    return core::ecs::TypeBuilder<AudioStop>("cubos::engine::AudioStop").build();
}
