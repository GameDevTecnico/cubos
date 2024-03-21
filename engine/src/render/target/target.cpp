#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/target/target.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderTarget)
{
    return core::ecs::TypeBuilder<RenderTarget>("cubos::engine::RenderTarget")
        .withField("size", &RenderTarget::size)
        .build();
}
