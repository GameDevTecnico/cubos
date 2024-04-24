#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/lights/environment.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderEnvironment)
{
    return core::ecs::TypeBuilder<RenderEnvironment>("cubos::engine::RenderEnvironment").build();
}
