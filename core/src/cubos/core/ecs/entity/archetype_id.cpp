#include <cstdint>

#include <cubos/core/ecs/entity/archetype_id.hpp>

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::ArchetypeIdHash;

const ArchetypeId ArchetypeId::Empty = {.inner = 0};
const ArchetypeId ArchetypeId::Invalid = {.inner = SIZE_MAX};

std::size_t ArchetypeIdHash::operator()(const ArchetypeId& id) const
{
    return id.inner;
}
