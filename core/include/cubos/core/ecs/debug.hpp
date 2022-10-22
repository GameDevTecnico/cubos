#ifndef CUBOS_CORE_ECS_DEBUG_HPP
#define CUBOS_CORE_ECS_DEBUG_HPP

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// Special type of query used for debugging purposes, which allows the system to access all entities and
    /// their components in a packaged form.
    /// @see cubos::core::data::Package
    class Debug
    {
    public:
        /// Used to iterate over the debug data.
        class Iterator
        {
        public:
            std::tuple<Entity, data::Package> operator*() const;
            bool operator==(const Iterator&) const;
            bool operator!=(const Iterator&) const;
            Iterator& operator++();

        private:
            friend Debug;

            const World& world;         ///< The world to query from.
            EntityManager::Iterator it; ///< The internal entity iterator.

            /// @param world The world to query from.
            /// @param it The internal entity iterator.
            Iterator(const World& world, EntityManager::Iterator it);
        };

        /// @param world The world to query.
        Debug(World& world);

        /// @return The iterator to the first entity.
        Iterator begin();

        /// @return The iterator to the end of the entity list.
        Iterator end();

        /// @brief Creates a package from the components of an entity.
        /// @param entity Entity ID.
        /// @returns A package containing the components of the entity.
        data::Package pack(Entity entity);

        /// @brief Unpacks components specified in a package into an entity.
        /// @param entity Entity ID.
        /// @param package Package to unpack.
        void unpack(Entity entity, const data::Package& package);

    private:
        World& world; ///< The world to query.
    };

} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_DEBUG_HPP
