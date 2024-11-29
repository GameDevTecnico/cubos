#include "listener_impl.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioListenerImpl)
{
    return core::ecs::TypeBuilder<AudioListenerImpl>("cubos::engine::AudioListenerImpl")
        .withField("index", &AudioListenerImpl::index)
        .build();
}
