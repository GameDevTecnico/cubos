#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/voxels/load.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::LoadRenderVoxels)
{
    return core::ecs::TypeBuilder<LoadRenderVoxels>("cubos::engine::LoadRenderVoxels").ephemeral().build();
}
