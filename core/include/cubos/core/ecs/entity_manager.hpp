#pragma once

#include <bitset>
#include <cstdint>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

namespace cubos::core::ecs
{
    /// Handle used to identify an Entity.
    class Entity
    {
    public:
        using Mask = std::bitset<CUBOS_CORE_ECS_MAX_COMPONENTS + 1>;

        Entity();

        /// @param index The index of the entity.
        /// @param generation The generation of the entity.
        Entity(uint32_t index, uint32_t generation);

        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;
        bool operator==(const Entity& /*entity*/) const;
        bool operator!=(const Entity& /*entity*/) const;

        /// Checks if the entity is 'none' (special value returned on errors).
        bool isNull() const;

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
            bool operator==(const Iterator& /*other*/) const;
            bool operator!=(const Iterator& /*other*/) const;
            Iterator& operator++();

        private:
            friend EntityManager;

            const EntityManager& mManager; ///< The entity manager being iterated.
            const Entity::Mask mMask;      ///< The mask of the components to be iterated.

            /// @param e The entity manager being iterated.
            /// @param m The mask of the components to be iterated.
            Iterator(const EntityManager& e, Entity::Mask m);
            Iterator(const EntityManager& e);

            std::unordered_map<Entity::Mask, std::set<uint32_t>>::const_iterator
                mArchetypeIt;                             ///< The current archetype iterator.
            std::set<uint32_t>::const_iterator mEntityIt; ///< The current entity iterator.
        };

        /// @param initialCapacity The initial capacity of the entity manager.
        EntityManager(std::size_t initialCapacity);
        ~EntityManager() = default;

        /// Creates a new entity with a certain component mask.
        /// @param mask The component mask of the entity.
        /// @returns The entity handle.
        Entity create(Entity::Mask mask);

        /// Removes an entity from the world.
        /// @param entity The entity to remove.
        void destroy(Entity entity);

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

        /// Returns an iterator over all entities.
        /// @returns An iterator over all entities.
        Iterator begin() const;

        /// Returns an iterator over all entities with a certain set of components.
        /// @param mask The mask of the components to be iterated.
        /// @returns An iterator over all entities with the given components.
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

        std::vector<EntityData> mEntities;                                ///< The pool of entities.
        std::queue<uint32_t> mAvailableEntities;                          ///< Queue with available entity indices.
        std::unordered_map<Entity::Mask, std::set<uint32_t>> mArchetypes; ///< Cache archetype entity indices.
    };
} // namespace cubos::core::ecs

namespace std
{
    /// Add hash function for Entity, so that it can be used as a key in an unordered_map.
    /// @cond
    template <>
    struct hash<cubos::core::ecs::Entity>
    {
        inline std::size_t operator()(const cubos::core::ecs::Entity& k) const
        {
            return hash<uint32_t>()(k.index);
        }
    };
    /// @endcond
} // namespace std
