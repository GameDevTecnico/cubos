#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/renderer/environment.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RendererEnvironment)
{
    return core::ecs::TypeBuilder<RendererEnvironment>("cubos::engine::RendererEnvironment").build();
}
