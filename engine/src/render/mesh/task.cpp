#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/mesh/task.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderMeshTask)
{
    return core::ecs::TypeBuilder<RenderMeshTask>("cubos::engine::RenderMeshTask").ephemeral().build();
}
