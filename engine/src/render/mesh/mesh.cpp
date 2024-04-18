#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/mesh/mesh.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderMesh)
{
    return core::ecs::TypeBuilder<RenderMesh>("cubos::engine::RenderMesh").build();
}
