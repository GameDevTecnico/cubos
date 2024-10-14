#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/collisions/shapes/voxel.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::VoxelCollisionShape)
{
    return core::ecs::TypeBuilder<VoxelCollisionShape>("cubos::engine::VoxelCollisionShape")
        .withField("grid", &VoxelCollisionShape::grid)
        .build();
}
