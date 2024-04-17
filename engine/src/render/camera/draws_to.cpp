#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/camera/draws_to.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::DrawsTo)
{
    return core::ecs::TypeBuilder<DrawsTo>("cubos::engine::DrawsTo").build();
}
