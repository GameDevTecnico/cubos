#include "source_impl.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioSourceImpl)
{
    return core::ecs::TypeBuilder<AudioSourceImpl>("cubos::engine::AudioSourceImpl").build();
}
