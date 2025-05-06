#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/collider_aabb.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ColliderAABB)
{
    return core::ecs::TypeBuilder<ColliderAABB>("cubos::engine::ColliderAABB").build();
}
