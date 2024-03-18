#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/depth/depth.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderDepth)
{
    return core::ecs::TypeBuilder<RenderDepth>("cubos::engine::RenderDepth")
        .withField("size", &RenderDepth::size)
        .build();
}
