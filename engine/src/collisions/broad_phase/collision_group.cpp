#include "collision_group.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CollisionGroup)
{
    return core::ecs::TypeBuilder<CollisionGroup>("cubos::engine::CollisionGroup")
        .withField("groupId", &CollisionGroup::groupId)
        .withField("isStatic", &CollisionGroup::isStatic)
        .build();
}
