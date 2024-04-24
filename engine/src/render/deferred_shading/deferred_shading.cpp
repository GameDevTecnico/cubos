#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/deferred_shading/deferred_shading.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::DeferredShading)
{
    return core::ecs::TypeBuilder<DeferredShading>("cubos::engine::DeferredShading").build();
}
