#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/render/camera/draws_to.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::DrawsTo)
{
    return core::ecs::TypeBuilder<DrawsTo>("cubos::engine::DrawsTo")
        .tree()
        .withField("viewportOffset", &DrawsTo::viewportOffset)
        .withField("viewportSize", &DrawsTo::viewportSize)
        .build();
}
