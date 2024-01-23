/// @file
/// @brief Class @ref cubos::core::ecs::Query.
/// @ingroup core-ecs-system

#pragma once

#include <cubos/core/ecs/query/data.hpp>

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
    /// @ingroup core-ecs-system
    template <typename... Ts>
    class Query
    {
    public:
        using Iterator = typename QueryData<Ts...>::View::Iterator;

        /// @brief Constructs.
        /// @param data Query data.
        Query(QueryData<Ts...>& data)
            : mView{data.view()}
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

    private:
        typename QueryData<Ts...>::View mView;
    };
} // namespace cubos::core::ecs
