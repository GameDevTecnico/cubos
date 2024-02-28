/// @file
/// @brief Class @ref cubos::core::ecs::Query.
/// @ingroup core-ecs-system-arguments

#pragma once

#include <cubos/core/ecs/query/data.hpp>
#include <cubos/core/ecs/system/access.hpp>
#include <cubos/core/ecs/system/fetcher.hpp>
#include <cubos/core/ecs/system/options.hpp>

namespace cubos::core::ecs
{
    /// @brief System argument which holds the result of a query over all entities in world which
    /// match the given arguments.
    ///
    /// An example of a valid query is:
    ///
    /// @code{.cpp}
    ///     Query<Position&, const Velocity&, Opt<Rotation&>, Opt<const Scale&>>
    /// @endcode
    ///
    /// This query will return all entities with a `Position` and `Velocity` component. Accessors
    /// to `Rotation` and `Scale` components are also passed but may be null if the component is
    /// not present in the entity. Whenever mutability is not needed, `const` should be used.
    ///
    /// @tparam Ts Argument types.
    /// @ingroup core-ecs-system-arguments
    template <typename... Ts>
    class Query
    {
    public:
        using Iterator = typename QueryData<Ts...>::View::Iterator;

        /// @brief Constructs.
        /// @param view Query data view.
        Query(typename QueryData<Ts...>::View view)
            : mView{view}
        {
        }

        /// @brief Returns an iterator pointing to the first query match.
        /// @return Iterator.
        Iterator begin()
        {
            return mView.begin();
        }

        /// @brief Returns an out of bounds iterator representing the end of the query matches.
        /// @return Iterator.
        Iterator end()
        {
            return mView.end();
        }

        /// @brief Returns a new query equal to this one but with the given target pinned to the given entity.
        ///
        /// Effectively this filters out all matches where the given target isn't the given entity.
        ///
        /// @param target Target index.
        /// @param entity Entity.
        /// @return Query.
        Query pin(int target, Entity entity)
        {
            return {mView.pin(target, entity)};
        }

        /// @brief Accesses the match for the given entity, if there is one.
        /// @param entity Entity.
        /// @return Requested components, or nothing if the entity does not match the query.
        Opt<std::tuple<Ts...>> at(Entity entity)
        {
            return mView.data().at(entity);
        }

        /// @brief Accesses the match for the given entities, if there is one.
        /// @param firstEntity Entity for the first target.
        /// @param secondEntity Entity for the second target.
        /// @return Requested data, or nothing if the entities do not match the query.
        Opt<std::tuple<Ts...>> at(Entity firstEntity, Entity secondEntity)
        {
            return mView.data().at(firstEntity, secondEntity);
        }

        /// @brief Returns the first match of the query, if there's any.
        /// @return Requested data, or nothing if there are no matches.
        Opt<std::tuple<Ts...>> first()
        {
            auto it = this->begin();
            if (it == this->end())
            {
                return {};
            }
            return *it;
        }

    private:
        typename QueryData<Ts...>::View mView;
    };

    template <typename... Ts>
    class SystemFetcher<Query<Ts...>>
    {
    public:
        static inline constexpr bool ConsumesOptions = true;

        SystemFetcher(World& world, const SystemOptions& options)
            : mData{world, options.queryTerms}
            , mObservedTarget{options.observedTarget}
        {
        }

        void analyze(SystemAccess& access) const
        {
            for (const auto& id : mData.accesses())
            {
                access.dataTypes.insert(id);
            }
        }

        Query<Ts...> fetch(const SystemContext& ctx)
        {
            mData.update();

            if (mObservedTarget != -1)
            {
                return {mData.view().pin(mObservedTarget, ctx.observedEntity)};
            }

            return {mData.view()};
        }

    private:
        QueryData<Ts...> mData;
        int mObservedTarget;
    };
} // namespace cubos::core::ecs
