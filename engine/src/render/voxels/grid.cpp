#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/render/voxels/grid.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderVoxelGrid)
{
    return core::ecs::TypeBuilder<RenderVoxelGrid>("cubos::engine::RenderVoxelGrid")
        .withField("asset", &RenderVoxelGrid::asset)
        .withField("offset", &RenderVoxelGrid::offset)
        .build();
}
