/// @file
/// @brief Class @ref cubos::core::ecs::EntityPool.
/// @ingroup core-ecs-entity

#pragma once

#include <queue>
#include <vector>

#include <cubos/core/ecs/entity/archetype_graph.hpp>
#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/entity/hash.hpp>

namespace cubos::core::ecs
{
    /// @brief Manages the creation and destruction of entity identifiers, as well as storing their
    /// archetype identifiers.
    /// @ingroup core-ecs-entity
    class CUBOS_CORE_API EntityPool final
    {
    public:
        /// @brief Creates a new entity on the given archetype.
        /// @param archetype Archetype identifier.
        /// @return Entity.
        Entity create(ArchetypeId archetype);

        /// @brief Removes an entity from the world.
        /// @param index Entity index to remove.
        void destroy(uint32_t index);

        /// @brief Changes the archetype identifier of an entity.
        /// @param index Entity index.
        /// @param archetype New archetype identifier.
        void archetype(uint32_t index, ArchetypeId archetype);

        /// @brief Gets the archetype identifier of an entity.
        /// @param index Entity index.
        /// @return Archetype identifier.
        ArchetypeId archetype(uint32_t index) const;

        /// @brief Gets the generation of an entity index.
        /// @param index Entity index.
        /// @return Generation.
        uint32_t generation(uint32_t index) const;

        /// @brief Checks if a given entity exists.
        /// @param entity Entity to check.
        /// @return Whether the entity exists.
        bool contains(Entity entity) const;

        /// @brief Gets the number of alive entities in the pool.
        /// @return Number of entities.
        size_t size() const;

    private:
        /// @brief Contains the state of an entity.
        struct Entry
        {
            uint32_t generation;   ///< Used to detect if the entity has been removed.
            ArchetypeId archetype; ///< Archetype identifier.
        };

        std::vector<Entry> mEntries; ///< Pool of entities.
        std::queue<uint32_t> mFree;  ///< Queue with free entity indices.
    };
} // namespace cubos::core::ecs
