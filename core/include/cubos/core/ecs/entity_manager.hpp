#ifndef CUBOS_CORE_ECS_ENTITY_MANAGER_HPP
#define CUBOS_CORE_ECS_ENTITY_MANAGER_HPP

#include <bitset>
#include <cstdint>
#include <queue>
#include <vector>
#include <set>
#include <unordered_map>

namespace cubos::core::ecs
{
    /// Handle used to identify an Entity.
    class Entity
    {
    public:
        using Mask = std::bitset<CUBOS_CORE_ECS_MAX_COMPONENTS>;

        Entity();
        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;

    private:
        friend class EntityManager;
        friend class World;

        /// @param index The index of the entity.
        /// @param generation The generation of the entity.
        Entity(uint32_t index, uint32_t generation);

        uint32_t index;      ///< Used as index in storages.
        uint32_t generation; ///< Used to detect if the entity has been removed.
    };

    /// Object which manages the entities of the ECS world and their masks.
    /// Used internally by the ECS world.
    class EntityManager final
    {
    public:
        /// Used to iterate over all entities of a world with a certain set of components.
        class Iterator
        {
        public:
            Entity operator*() const;
            bool operator==(const Iterator&) const;
            bool operator!=(const Iterator&) const;
            Iterator& operator++();

        private:
            friend EntityManager;

            const EntityManager& manager; ///< The entity manager being iterated.
            const Entity::Mask mask;      ///< The mask of the components to be iterated.

            /// @param e The entity manager being iterated.
            /// @param m The mask of the components to be iterated.
            Iterator(const EntityManager& e, const Entity::Mask m);
            Iterator(const EntityManager& e);

            std::unordered_map<Entity::Mask, std::set<uint32_t>>::const_iterator
                archetypeIt;                             ///< The current archetype iterator.
            std::set<uint32_t>::const_iterator entityIt; ///< The current entity iterator.
        };

        /// @param initialCapacity The initial capacity of the entity manager.
        EntityManager(size_t initialCapacity);
        ~EntityManager() = default;

        /// Creates a new entity with a certain component mask.
        /// @param mask The component mask of the entity.
        /// @returns The entity handle.
        Entity create(Entity::Mask mask);

        /// Removes an entity from the world.
        /// @param entity The entity to remove.
        void remove(Entity entity);

        /// Sets the component mask of an entity.
        /// @param entity The entity to set the mask of.
        /// @param mask The mask to set.
        void setMask(Entity entity, Entity::Mask mask);

        /// Gets the component mask of an entity.
        /// @param entity The entity to get the mask of.
        /// @returns The component mask of the entity.
        const Entity::Mask& getMask(Entity entity) const;

        /// Checks if an entity is still valid.
        /// @param entity The entity to check.
        /// @returns True if the entity is still valid, false otherwise.
        bool isValid(Entity entity) const;

        /// Returns an iterator over all entities of a world with a certain set of components.
        /// @param world The world to iterate over.
        /// @param mask The mask of the components to be iterated.
        /// @returns An iterator over all entities of the world with the given components.
        Iterator withMask(Entity::Mask mask) const;

        /// Returns an iterator which points to the end of the entity manager.
        /// @returns An iterator which points to the end of the entity manager.
        Iterator end() const;

    private:
        /// Internal data struct containing the state of an entity.
        struct EntityData
        {
            uint32_t generation; ///< Used to detect if the entity has been removed.
            Entity::Mask mask;   ///< The component mask of the entity.
        };

        std::vector<EntityData> entities;                                ///< The pool of entities.
        std::queue<uint32_t> availableEntities;                          ///< Queue with available entity indices.
        std::unordered_map<Entity::Mask, std::set<uint32_t>> archetypes; ///< Cache archetype entity indices.
    };
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_ENTITY_MANAGER_HPP
