#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/local_to_world.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::LocalToWorld)
{
    return core::ecs::ComponentTypeBuilder<LocalToWorld>("cubos::engine::LocalToWorld")
        .withField("mat", &LocalToWorld::mat)
        .build();
}
