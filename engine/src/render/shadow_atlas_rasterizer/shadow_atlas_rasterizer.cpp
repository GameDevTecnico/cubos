#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/shadow_atlas_rasterizer/shadow_atlas_rasterizer.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ShadowAtlasRasterizer)
{
    return core::ecs::TypeBuilder<ShadowAtlasRasterizer>("cubos::engine::ShadowAtlasRasterizer").build();
}
