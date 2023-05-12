#pragma once

#include <optional>
#include <typeindex>
#include <unordered_set>
#include <utility>

#include <cubos/core/ecs/accessors.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// Contains information about a query.
    struct QueryInfo
    {
        std::unordered_set<std::type_index> read;    ///< The type set of components the query reads.
        std::unordered_set<std::type_index> written; ///< The type set of components the query writes.
    };

    /// This namespace contains functions used internally by the implementation
    /// of the ECS.
    namespace impl
    {
        /// Fetches the requested type from a world.
        template <typename T>
        struct QueryFetcher
        {
            // This should never be instantiated.
            static_assert(!std::is_same_v<T, T>,
                          "Unknown query argument type (must be either a reference or a pointer).");
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component>
        struct QueryFetcher<Write<Component>>
        {
            using Type = WriteStorage<Component>;
            using InnerType = Component;

            /// Whether the component is optional
            constexpr static bool IsOptional = false;

            /// Adds the component to the query info structure.
            /// @param info The query info structure to add to.
            static void add(QueryInfo& info);

            /// @param world The world to fetch from.
            /// @returns The requested component write lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the reference from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component reference.
            static Write<Component> arg(const World& world, Type& lock, Entity entity);
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component>
        struct QueryFetcher<Read<Component>>
        {
            using Type = ReadStorage<Component>;
            using InnerType = Component;

            /// Whether the component is optional
            constexpr static bool IsOptional = false;

            /// Adds the component to the query info structure.
            /// @param info The query info structure to add to.
            static void add(QueryInfo& info);

            /// @param world The world to fetch from.
            /// @returns The requested component read lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the reference from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component reference.
            static Read<Component> arg(const World& world, Type& lock, Entity entity);
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component>
        struct QueryFetcher<OptWrite<Component>>
        {
            using Type = WriteStorage<Component>;
            using InnerType = Component;

            /// Whether the component is optional
            constexpr static bool IsOptional = true;

            /// Adds the component to the query info structure.
            /// @param info The query info structure to add to.
            static void add(QueryInfo& info);

            /// @param world The world to fetch from.
            /// @returns The requested component write lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the pointer from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component pointer.
            static OptWrite<Component> arg(const World& world, Type& lock, Entity entity);
        };

        /// @tparam Component The type of the component to fetch.
        template <typename Component>
        struct QueryFetcher<OptRead<Component>>
        {
            using Type = ReadStorage<Component>;
            using InnerType = Component;

            /// Whether the component is optional
            constexpr static bool IsOptional = true;

            /// Adds the component to the query info structure.
            /// @param info The query info structure to add to.
            static void add(QueryInfo& info);

            /// @param world The world to fetch from.
            /// @returns The requested component read lock.
            static Type fetch(const World& world);

            /// @param lock The component lock to get the pointer from.
            /// @param entity The entity to get the component from.
            /// @returns The requested component pointer.
            static OptRead<Component> arg(const World& world, Type& lock, Entity entity);
        };
    } // namespace impl

    /// Contains the result of a query over all entities with some given components.
    ///
    /// An example of a valid query is:
    ///
    ///     Query<Write<Position>, Read<Velocity>, OptWrite<Rotation>, OptRead<Scale>>
    ///
    /// This query will return all entities with a Position and Velocity component. Accessors to Rotation and Scale
    /// components are also passed but may be null, if the component is not present in the entity. Whenever mutability
    /// is not needed, Read/OptRead should be used.
    ///
    /// @tparam ComponentTypes The types of the component accessors to be queried.
    template <typename... ComponentTypes>
    class Query
    {
    public:
        using Fetched = std::tuple<typename impl::QueryFetcher<ComponentTypes>::Type...>;

        /// Used to iterate over the results of a query.
        class Iterator
        {
        public:
            std::tuple<Entity, ComponentTypes...> operator*() const;
            bool operator==(const Iterator& /*other*/) const;
            bool operator!=(const Iterator& /*other*/) const;
            Iterator& operator++();

        private:
            friend Query<ComponentTypes...>;

            const World& mWorld;         ///< The world to query from.
            Fetched& mFetched;           ///< The fetched data.
            EntityManager::Iterator mIt; ///< The internal entity iterator.

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

        /// Gets information about the query.
        /// @returns The query information.
        static QueryInfo info();

    private:
        friend World;

        const World& mWorld; ///< The world to query.
        Fetched mFetched;    ///< The fetched data.
        Entity::Mask mMask;  ///< The mask of the components to query.
    };

    // Implementation.

    template <typename... ComponentTypes>
    std::tuple<Entity, ComponentTypes...> Query<ComponentTypes...>::Iterator::operator*() const
    {

        // Convert the fetched data into the desired query reference types.
        return std::forward_as_tuple(
            *mIt, impl::QueryFetcher<ComponentTypes>::arg(
                      mWorld, std::get<typename impl::QueryFetcher<ComponentTypes>::Type>(mFetched), *mIt)...);
    }

    template <typename... ComponentTypes>
    bool Query<ComponentTypes...>::Iterator::operator==(const Iterator& other) const
    {
        return mIt == other.mIt;
    }

    template <typename... ComponentTypes>
    bool Query<ComponentTypes...>::Iterator::operator!=(const Iterator& other) const
    {
        return mIt != other.mIt;
    }

    template <typename... ComponentTypes>
    typename Query<ComponentTypes...>::Iterator& Query<ComponentTypes...>::Iterator::operator++()
    {
        ++mIt;
        return *this;
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...>::Iterator::Iterator(const World& world, Fetched& fetched, EntityManager::Iterator it)
        : mWorld(world)
        , mFetched(fetched)
        , mIt(std::move(it))
    {
        // Nothing to do.
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...>::Query(const World& world)
        : mWorld(world)
        , mFetched(std::forward_as_tuple(impl::QueryFetcher<ComponentTypes>::fetch(world)...))
    {
        // We must turn the type from Read<T> and similar to T before getting the ID.
        std::size_t ids[] = {(impl::QueryFetcher<ComponentTypes>::IsOptional
                                  ? SIZE_MAX
                                  : mWorld.mComponentManager
                                        .template getID<typename impl::QueryFetcher<ComponentTypes>::InnerType>())...};
        mMask.reset();
        mMask.set(0);
        for (std::size_t id : ids)
        {
            if (id != SIZE_MAX)
            {
                mMask.set(id);
            }
        }
    }

    template <typename... ComponentTypes>
    typename Query<ComponentTypes...>::Iterator Query<ComponentTypes...>::begin()
    {
        return Iterator(mWorld, mFetched, mWorld.mEntityManager.withMask(mMask));
    }

    template <typename... ComponentTypes>
    typename Query<ComponentTypes...>::Iterator Query<ComponentTypes...>::end()
    {
        return Iterator(mWorld, mFetched, mWorld.mEntityManager.end());
    }

    template <typename... ComponentTypes>
    QueryInfo Query<ComponentTypes...>::info()
    {
        QueryInfo info;
        ([&]() { impl::QueryFetcher<ComponentTypes>::add(info); }(), ...);
        return info;
    }

    template <typename Component>
    void impl::QueryFetcher<Write<Component>>::add(QueryInfo& info)
    {
        info.written.insert(typeid(Component));
    }

    template <typename Component>
    typename impl::QueryFetcher<Write<Component>>::Type impl::QueryFetcher<Write<Component>>::fetch(const World& world)
    {
        return world.mComponentManager.write<Component>();
    }

    template <typename Component>
    Write<Component> impl::QueryFetcher<Write<Component>>::arg(const World& /*unused*/, Type& lock, Entity entity)
    {
        return {*lock.get().get(entity.index)};
    }

    template <typename Component>
    void impl::QueryFetcher<Read<Component>>::add(QueryInfo& info)
    {
        info.read.insert(typeid(Component));
    }

    template <typename Component>
    typename impl::QueryFetcher<Read<Component>>::Type impl::QueryFetcher<Read<Component>>::fetch(const World& world)
    {
        return world.mComponentManager.read<Component>();
    }

    template <typename Component>
    Read<Component> impl::QueryFetcher<Read<Component>>::arg(const World& /*unused*/, Type& lock, Entity entity)
    {
        return {*lock.get().get(entity.index)};
    }

    template <typename Component>
    void impl::QueryFetcher<OptWrite<Component>>::add(QueryInfo& info)
    {
        info.written.insert(typeid(Component));
    }

    template <typename Component>
    typename impl::QueryFetcher<OptWrite<Component>>::Type impl::QueryFetcher<OptWrite<Component>>::fetch(
        const World& world)
    {
        return world.mComponentManager.write<Component>();
    }

    template <typename Component>
    OptWrite<Component> impl::QueryFetcher<OptWrite<Component>>::arg(const World& world, Type& lock, Entity entity)
    {
        if (world.has<Component>(entity))
        {
            return {lock.get().get(entity)};
        }

        return {nullptr};
    }

    template <typename Component>
    void impl::QueryFetcher<OptRead<Component>>::add(QueryInfo& info)
    {
        info.read.insert(typeid(Component));
    }

    template <typename Component>
    typename impl::QueryFetcher<OptRead<Component>>::Type impl::QueryFetcher<OptRead<Component>>::fetch(
        const World& world)
    {
        return world.mComponentManager.read<Component>();
    }

    template <typename Component>
    OptRead<Component> impl::QueryFetcher<OptRead<Component>>::arg(const World& world, Type& lock, Entity entity)
    {
        if (world.has<Component>(entity))
        {
            return {lock.get().get(entity.index)};
        }

        return {nullptr};
    }

    template <typename... ComponentTypes>
    std::optional<std::tuple<ComponentTypes...>> Query<ComponentTypes...>::operator[](Entity entity)
    {
        auto mask = mWorld.mEntityManager.getMask(entity);
        if ((mask & mMask) == mMask)
        {
            return std::forward_as_tuple(impl::QueryFetcher<ComponentTypes>::arg(
                mWorld, std::get<typename impl::QueryFetcher<ComponentTypes>::Type>(mFetched), entity)...);
        }

        return std::nullopt;
    }

} // namespace cubos::core::ecs
