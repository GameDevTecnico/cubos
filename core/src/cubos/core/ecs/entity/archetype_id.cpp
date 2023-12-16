#include <cstdint>

#include <cubos/core/ecs/entity/archetype_id.hpp>

using cubos::core::ecs::ArchetypeId;

const ArchetypeId ArchetypeId::Empty = {.inner = 0};
const ArchetypeId ArchetypeId::Invalid = {.inner = SIZE_MAX};
