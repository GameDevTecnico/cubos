/// @file
/// @brief Class @ref cubos::core::ecs::Query and related types.
///
/// This file is a bit scary, but it's not as bad as it looks. It's mostly template specializations
/// to handle the different types of arguments a query can take.
///
/// @ingroup core-ecs-system

#pragma once

#include <optional>
#include <typeindex>
#include <unordered_set>
#include <utility>

#include <cubos/core/ecs/system/accessors.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// @brief Describes a query.
    /// @ingroup core-ecs-system
    struct QueryInfo
    {
        std::unordered_set<std::type_index> read;    ///< Componenst read.
        std::unordered_set<std::type_index> written; ///< Components written.
    };

    namespace impl
    {
        /// @brief Fetches the requested data from a world.
        ///
        /// Each possible accessor type is specialized to provide the correct data.
        ///
        /// @tparam T Query argument type.
        template <typename T>
        struct QueryFetcher
        {
            // This should never be instantiated.
            static_assert(!std::is_same_v<T, T>, "Unknown query argument type.");
        };

        template <typename Component>
        struct QueryFetcher<Write<Component>>
        {
            using Type = WriteStorage<Component>;
            using InnerType = Component;

            constexpr static bool IsOptional = false;
            static void add(QueryInfo& info);
            static Type fetch(const World& world);
            static Write<Component> arg(const World& world, Type& lock, Entity entity);
        };

        template <typename Component>
        struct QueryFetcher<Read<Component>>
        {
            using Type = ReadStorage<Component>;
            using InnerType = Component;

            constexpr static bool IsOptional = false;
            static void add(QueryInfo& info);
            static Type fetch(const World& world);
            static Read<Component> arg(const World& world, Type& lock, Entity entity);
        };

        template <typename Component>
        struct QueryFetcher<OptWrite<Component>>
        {
            using Type = WriteStorage<Component>;
            using InnerType = Component;

            constexpr static bool IsOptional = true;
            static void add(QueryInfo& info);
            static Type fetch(const World& world);
            static OptWrite<Component> arg(const World& world, Type& lock, Entity entity);
        };

        template <typename Component>
        struct QueryFetcher<OptRead<Component>>
        {
            using Type = ReadStorage<Component>;
            using InnerType = Component;

            constexpr static bool IsOptional = true;
            static void add(QueryInfo& info);
            static Type fetch(const World& world);
            static OptRead<Component> arg(const World& world, Type& lock, Entity entity);
        };
    } // namespace impl

    /// @brief System argument which holds the result of a query over all entities in world which
    /// match the given arguments.
    ///
    /// An example of a valid query is:
    ///
    /// @code{.cpp}
    ///     Query<Write<Position>, Read<Velocity>, OptWrite<Rotation>, OptRead<Scale>>
    /// @endcode
    ///
    /// This query will return all entities with a `Position` and `Velocity` component. Accessors
    /// to `Rotation` and `Scale` components are also passed but may be null if the component is
    /// not present in the entity. Whenever mutability is not needed, Read/OptRead should be used.
    ///
    /// @tparam ComponentTypes Component accessor types to be queried.
    /// @ingroup core-ecs-system
    template <typename... ComponentTypes>
    class Query
    {
    public:
        using Fetched = std::tuple<typename impl::QueryFetcher<ComponentTypes>::Type...>;

        /// @brief Used to iterate over the results of a query.
        class Iterator
        {
        public:
            /// @brief Dereferences to a tuple containing the queried entity and its components.
            /// @return Tuple containing the entity and its components.
            std::tuple<Entity, ComponentTypes...> operator*() const;

            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;
            Iterator& operator++();

        private:
            friend Query<ComponentTypes...>;

            const World& mWorld;         ///< World to query from.
            Fetched& mFetched;           ///< Fetched data.
            EntityManager::Iterator mIt; ///< Internal entity iterator.

            /// @param world World to query from.
            /// @param fetched Fetched data.
            /// @param it Internal entity iterator.
            Iterator(const World& world, Fetched& fetched, EntityManager::Iterator it);
        };

        /// @brief Constructs a query over the given world.
        /// @param world World to query.
        Query(const World& world);

        /// @brief Gets an iterator to the first entity which matches the query.
        /// @return Iterator.
        Iterator begin();

        /// @brief Gets an iterator to the end of the query.
        /// @return Iterator.
        Iterator end();

        /// @brief Accesses an entity's components directly, without iterating over the query.
        /// @param entity Entity to access.
        /// @return Requested components, or std::nullopt if the entity does not match the query.
        std::optional<std::tuple<ComponentTypes...>> operator[](Entity entity);

        /// @brief Gets information about the query.
        /// @return Query information.
        static QueryInfo info();

    private:
        friend World;

        const World& mWorld; ///< World to query.
        Fetched mFetched;    ///< Fetched data.
        Entity::Mask mMask;  ///< Mask of the components to query.
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
        std::size_t ids[] = {0,
                             (impl::QueryFetcher<ComponentTypes>::IsOptional
                                  ? SIZE_MAX
                                  : mWorld.mComponentManager
                                        .template getID<typename impl::QueryFetcher<ComponentTypes>::InnerType>())...};
        mMask.reset();
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
            return {lock.get().get(entity.index)};
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
