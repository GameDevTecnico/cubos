#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/g_buffer/g_buffer.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::GBuffer)
{
    return core::ecs::TypeBuilder<GBuffer>("cubos::engine::GBuffer")
        .withField("size", &GBuffer::size)
        .withField("cleared", &GBuffer::cleared)
        .build();
}
