#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/local_to_world.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::LocalToWorld)
{
    return core::ecs::TypeBuilder<LocalToWorld>("cubos::engine::LocalToWorld")
        .ephemeral()
        .withField("mat", &LocalToWorld::mat)
        .build();
}
