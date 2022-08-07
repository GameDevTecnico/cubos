#ifndef CUBOS_CORE_ECS_QUERY_HPP
#define CUBOS_CORE_ECS_QUERY_HPP

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// This namespace contains functions used internally by the implementation
    /// of the ECS.
    namespace impl
    {
        /// Fetches the requested type from a world.
        template <typename T> struct QueryFetcher
        {
            // This should never be instantiated.
            static_assert(!std::is_same_v<T, T>,
                          "Unknown query argument type (must be either a reference or a pointer).");
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component> struct QueryFetcher<Component&>
        {
            using Type = WriteStorage<Component>;

            /// Whether the component is optional
            constexpr static bool IS_OPTIONAL = false;

            /// @param world The world to fetch from.
            /// @returns The requested component write lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the reference from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component reference.
            static Component& arg(const World& world, Type& lock, Entity entity);
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component> struct QueryFetcher<const Component&>
        {
            using Type = ReadStorage<Component>;

            /// Whether the component is optional
            constexpr static bool IS_OPTIONAL = false;

            /// @param world The world to fetch from.
            /// @returns The requested component read lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the reference from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component reference.
            static const Component& arg(const World& world, Type& lock, Entity entity);
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component> struct QueryFetcher<Component*>
        {
            using Type = WriteStorage<Component>;

            /// Whether the component is optional
            constexpr static bool IS_OPTIONAL = true;

            /// @param world The world to fetch from.
            /// @returns The requested component write lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the pointer from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component pointer.
            static Component* arg(const World& world, Type& lock, Entity entity);
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component> struct QueryFetcher<const Component*>
        {
            using Type = ReadStorage<Component>;

            /// Whether the component is optional
            constexpr static bool IS_OPTIONAL = true;

            /// @param world The world to fetch from.
            /// @returns The requested component read lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the pointer from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component pointer.
            static const Component* arg(const World& world, Type& lock, Entity entity);
        };
    } // namespace impl

    /// Contains the result of a query over all entities with some given components.
    ///
    /// An example of a valid query is:
    ///
    ///     Query<Position&, const Velocity&, Rotation*, const Scale*>
    ///
    /// This query will return all entities with a Position and Velocity component. Pointers to Rotation and Scale
    /// components are also passed but may be null, if the component is not present in the entity. Whenever mutability
    /// is not needed, a const reference/pointer should be used.
    ///
    /// @tparam ComponentTypes The types of the component references to be queried.
    template <typename... ComponentTypes> class Query
    {
    public:
        using Fetched = std::tuple<typename impl::QueryFetcher<ComponentTypes>::Type...>;

        /// Used to iterate over the results of a query.
        class Iterator
        {
        public:
            std::tuple<Entity, ComponentTypes...> operator*() const;
            bool operator==(const Iterator&) const;
            bool operator!=(const Iterator&) const;
            Iterator& operator++();

        private:
            friend Query<ComponentTypes...>;

            const World& world;         ///< The world to query from.
            Fetched& fetched;           ///< The fetched data.
            EntityManager::Iterator it; ///< The internal entity iterator.

            /// @param world The world to query from.
            /// @param fetched The fetched data.
            /// @param it The internal entity iterator.
            Iterator(const World& world, Fetched& fetched, EntityManager::Iterator it);
        };

        /// @param world The world to query.
        Query(const World& world);

        /// @return The iterator to the first entity with the given components.
        Iterator begin();

        /// @return The iterator to the end of the entity list.
        Iterator end();

        /// @param entity The entity to get the components from.
        /// @returns The requested components.
        std::optional<std::tuple<ComponentTypes...>> operator[](Entity entity);

    private:
        friend World;

        const World& world; ///< The world to query.
        Fetched fetched;    ///< The fetched data.
        Entity::Mask mask;  ///< The mask of the components to query.
    };

    // Implementation.

    template <typename... ComponentTypes>
    std::tuple<Entity, ComponentTypes...> Query<ComponentTypes...>::Iterator::operator*() const
    {

        // Convert the fetched data into the desired query reference types.
        return std::forward_as_tuple(
            *this->it,
            impl::QueryFetcher<ComponentTypes>::arg(
                this->world, std::get<typename impl::QueryFetcher<ComponentTypes>::Type>(this->fetched), *this->it)...);
    }

    template <typename... ComponentTypes>
    bool Query<ComponentTypes...>::Iterator::operator==(const Iterator& other) const
    {
        return this->it == other.it;
    }

    template <typename... ComponentTypes>
    bool Query<ComponentTypes...>::Iterator::operator!=(const Iterator& other) const
    {
        return this->it != other.it;
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...>::Iterator& Query<ComponentTypes...>::Iterator::operator++()
    {
        ++this->it;
        return *this;
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...>::Iterator::Iterator(const World& world, Fetched& fetched, EntityManager::Iterator it)
        : world(world), fetched(fetched), it(it)
    {
        // Nothing to do.
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...>::Query(const World& world)
        : world(world), fetched(std::forward_as_tuple(impl::QueryFetcher<ComponentTypes>::fetch(world)...))
    {
        // We must turn the type from const T& and similar to T before getting the ID.
        size_t ids[] = {
            (impl::QueryFetcher<ComponentTypes>::IS_OPTIONAL
                 ? SIZE_MAX
                 : this->world.componentManager.getID<
                       std::remove_const_t<std::remove_reference_t<std::remove_pointer_t<ComponentTypes>>>>())...};
        this->mask.reset();
        this->mask.set(0);
        for (size_t id : ids)
        {
            if (id != SIZE_MAX)
            {
                this->mask.set(id);
            }
        }
    }

    template <typename... ComponentTypes> Query<ComponentTypes...>::Iterator Query<ComponentTypes...>::begin()
    {
        return Iterator(this->world, this->fetched, this->world.entityManager.withMask(this->mask));
    }

    template <typename... ComponentTypes> Query<ComponentTypes...>::Iterator Query<ComponentTypes...>::end()
    {
        return Iterator(this->world, this->fetched, this->world.entityManager.end());
    }

    template <typename Component>
    typename impl::QueryFetcher<Component&>::Type impl::QueryFetcher<Component&>::fetch(const World& world)
    {
        return world.componentManager.write<Component>();
    }

    template <typename Component>
    Component& impl::QueryFetcher<Component&>::arg(const World& world, Type& lock, Entity entity)
    {
        return *lock.get().get(entity.index);
    }

    template <typename Component>
    typename impl::QueryFetcher<const Component&>::Type impl::QueryFetcher<const Component&>::fetch(const World& world)
    {
        return world.componentManager.read<Component>();
    }

    template <typename Component>
    const Component& impl::QueryFetcher<const Component&>::arg(const World& world, Type& lock, Entity entity)
    {
        return *lock.get().get(entity.index);
    }

    template <typename Component>
    typename impl::QueryFetcher<Component*>::Type impl::QueryFetcher<Component*>::fetch(const World& world)
    {
        return world.componentManager.write<Component>();
    }

    template <typename Component>
    Component* impl::QueryFetcher<Component*>::arg(const World& world, Type& lock, Entity entity)
    {
        if (world.has<Component>(entity))
        {
            return lock.get().get(entity);
        }
        else
        {
            return nullptr;
        }
    }

    template <typename Component>
    typename impl::QueryFetcher<const Component*>::Type impl::QueryFetcher<const Component*>::fetch(const World& world)
    {
        return world.componentManager.read<Component>();
    }

    template <typename Component>
    const Component* impl::QueryFetcher<const Component*>::arg(const World& world, Type& lock, Entity entity)
    {
        if (world.has<Component>(entity))
        {
            return lock.get().get(entity.index);
        }
        else
        {
            return nullptr;
        }
    }

    template <typename... ComponentTypes>
    std::optional<std::tuple<ComponentTypes...>> Query<ComponentTypes...>::operator[](Entity entity)
    {
        auto mask = this->world.entityManager.getMask(entity);
        if ((mask & this->mask) == this->mask)
        {
            return std::forward_as_tuple(impl::QueryFetcher<ComponentTypes>::arg(
                this->world, std::get<typename impl::QueryFetcher<ComponentTypes>::Type>(this->fetched), entity)...);
        }
        else
        {
            return std::nullopt;
        }
    }

} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_QUERY_HPP
