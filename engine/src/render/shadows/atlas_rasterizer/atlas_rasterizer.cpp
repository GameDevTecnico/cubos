#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/shadows/atlas_rasterizer/atlas_rasterizer.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ShadowAtlasRasterizer)
{
    return core::ecs::TypeBuilder<ShadowAtlasRasterizer>("cubos::engine::ShadowAtlasRasterizer").build();
}
