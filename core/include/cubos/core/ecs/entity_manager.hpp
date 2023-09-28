/// @file
/// @brief Class @ref cubos::core::ecs::EntityManager and related types.
/// @ingroup core-ecs

#pragma once

#include <bitset>
#include <cstdint>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

namespace cubos::core::ecs
{
    /// @brief Identifies an entity.
    ///
    /// When serializing/deserializing, if there's a
    /// data::old::SerializationMap<Entity, std::string> in the context, it will be used to
    /// (de)serialize strings representing the entities. Otherwise, the identifiers will be
    /// (de)serialized as objects with two fields: their index and their generation.
    ///
    /// @ingroup core-ecs
    class Entity
    {
    public:
        /// @brief Type used to store which components an entity has.
        using Mask = std::bitset<CUBOS_CORE_ECS_MAX_COMPONENTS + 1>;

        Entity();

        /// @brief Constructs an entity from an index and a generation.
        /// @param index Entity index.
        /// @param generation Entity generation.
        Entity(uint32_t index, uint32_t generation);

        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;
        bool operator==(const Entity& /*entity*/) const;
        bool operator!=(const Entity& /*entity*/) const;

        /// @brief Checks if the entity is null, a special value returned on errors.
        /// @return Whether the entity is null.
        bool isNull() const;

        uint32_t index;      ///< Index in storages.
        uint32_t generation; ///< Allows us to detect if the entity has been removed.
    };

    /// @brief Holds and manages entities and their component masks.
    ///
    /// Used internally by @ref World.
    ///
    /// @ingroup core-ecs
    class EntityManager final
    {
    public:
        /// @brief Used to iterate over all entities in a manager with a certain component mask.
        class Iterator
        {
        public:
            Entity operator*() const;
            bool operator==(const Iterator& /*other*/) const;
            bool operator!=(const Iterator& /*other*/) const;
            Iterator& operator++();

        private:
            friend EntityManager;

            const EntityManager& mManager; ///< Entity manager being iterated.
            const Entity::Mask mMask;      ///< Mask of the components to be iterated.

            /// @param e Entity manager being iterated.
            /// @param m Mask of the components to be iterated.
            Iterator(const EntityManager& e, Entity::Mask m);
            Iterator(const EntityManager& e);

            std::unordered_map<Entity::Mask, std::set<uint32_t>>::const_iterator
                mArchetypeIt;                             ///< Current archetype iterator.
            std::set<uint32_t>::const_iterator mEntityIt; ///< Current entity iterator.
        };

        /// @brief Constructs with a certain initial entity capacity.
        /// @param initialCapacity Initial capacity of the entity manager.
        EntityManager(std::size_t initialCapacity);
        ~EntityManager() = default;

        /// @brief Creates a new entity with a certain component mask.
        /// @param mask Component mask of the entity.
        /// @return Entity handle.
        Entity create(Entity::Mask mask);

        /// @brief Removes an entity from the world.
        /// @param entity Entity to remove.
        void destroy(Entity entity);

        /// @brief Sets the component mask of an entity.
        /// @param entity Entity to set the mask of.
        /// @param mask Mask to set.
        void setMask(Entity entity, Entity::Mask mask);

        /// @brief Gets the component mask of an entity.
        /// @param entity Entity to get the mask of.
        /// @return Component mask of the entity.
        const Entity::Mask& getMask(Entity entity) const;

        /// @brief Checks if an entity is still valid.
        ///
        /// Different from isAlive, as it will return true for entities which still have not been
        /// commited.
        ///
        /// @param entity Entity to check.
        /// @return Whether the entity is valid.
        bool isValid(Entity entity) const;

        /// @brief Checks if an entity is alive.
        /// @param entity Entity to check.
        /// @return Whether the entity is alive.
        bool isAlive(Entity entity) const;

        /// @brief Returns an iterator over all entities.
        /// @return Iterator over all entities.
        Iterator begin() const;

        /// @brief Returns an iterator over all entities with a certain mask of components.
        /// @param mask Mask of the components to be iterated.
        /// @return Iterator over all entities with the given component mask.
        Iterator withMask(Entity::Mask mask) const;

        /// @brief Returns an iterator which points to the end of the entity manager.
        /// @return Iterator which points to the end of the entity manager.
        Iterator end() const;

    private:
        /// @brief Internal data struct containing the state of an entity.
        struct EntityData
        {
            uint32_t generation; ///< Used to detect if the entity has been removed.
            Entity::Mask mask;   ///< Component mask of the entity.
        };

        std::vector<EntityData> mEntities;                                ///< Pool of entities.
        std::queue<uint32_t> mAvailableEntities;                          ///< Queue with available entity indices.
        std::unordered_map<Entity::Mask, std::set<uint32_t>> mArchetypes; ///< Cache archetype entity indices.
    };
} // namespace cubos::core::ecs

namespace std
{
    // Add hash function for Entity, so that it can be used as a key in an unordered_map.

    template <>
    struct hash<cubos::core::ecs::Entity>
    {
        inline std::size_t operator()(const cubos::core::ecs::Entity& k) const
        {
            return hash<uint32_t>()(k.index);
        }
    };
} // namespace std
