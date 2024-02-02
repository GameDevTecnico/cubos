#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/entity/hash.hpp>

using cubos::core::ecs::EntityHash;

std::size_t EntityHash::operator()(const Entity& entity) const noexcept
{
    // If std::size_t is 64 bits or more, we can fit both the index and generation in it.
    // Otherwise, we'll just use the index as the hash, which means we'll always get collisions
    // for entities with the same index but different generations. That shouldn't be a problem
    // though, since that use case is very unlikely.

#if SIZE_MAX <= UINT32_MAX
    return static_cast<std::size_t>(entity.index);
#else
    static_assert(sizeof(std::size_t) >= sizeof(uint32_t) * 2);
    return static_cast<std::size_t>(entity.index) |
           (static_cast<std::size_t>(entity.generation) << (sizeof(uint32_t) * 8));
#endif
}
