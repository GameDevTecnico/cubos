#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/g_buffer_rasterizer/g_buffer_rasterizer.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::GBufferRasterizer)
{
    return core::ecs::TypeBuilder<GBufferRasterizer>("cubos::engine::GBufferRasterizer").build();
}
