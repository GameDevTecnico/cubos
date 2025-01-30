#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/voxels/palette.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderPalette)
{
    return core::ecs::TypeBuilder<RenderPalette>("cubos::engine::RenderPalette")
        .wrap(&RenderPalette::asset);
}
