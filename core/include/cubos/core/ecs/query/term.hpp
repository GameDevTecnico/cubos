/// @file
/// @brief Struct @ref cubos::core::ecs::QueryTerm.
/// @ingroup core-ecs-query

#pragma once

#include <vector>

#include <cubos/core/ecs/types.hpp>

namespace cubos::core::ecs
{
    /// @brief Describes a term in a query.
    ///
    /// Terms are the building blocks used to build queries. Each term adds some restriction to what the query will
    /// match. There are three kinds of terms:
    /// - Component terms, which allow filtering match entities by those which have or don't have a given component.
    /// - Relation terms, which allow filtering match entities by some relation between them.
    /// - Entity terms, which don't filter the results but allow accessing one of the matched entities.
    ///
    /// @ingroup core-ecs-query
    struct QueryTerm
    {
        /// @brief Type of the data matched by the term.
        ///
        /// If this is set to @ref DataTypeId::Invalid, then the term is an entity term.
        /// Otherwise, the term can either be a component or relation term depending on this type.
        DataTypeId type;

        union {
            /// @brief Entity term data.
            struct
            {
                int target; ///< Index of the target accessed by this term.
            } entity;

            /// @brief Component term data.
            struct
            {
                int target;    ///< Index of the target accessed by this term.
                bool without;  ///< If true, requires the target to not have the component.
                bool optional; ///< If true, the term accesses the component but does not require it to be present.
            } component;
        };

        /// @brief Returns a new entity term for the given target.
        /// @param target Index of the target which will be accessed.
        /// @return Entity term.
        static QueryTerm makeEntity(int target);

        /// @brief Returns a new component term for the given component and target.
        /// @warning Undefined behaviour will occur if @p type isn't registered as a component.
        /// @param type Component type.
        /// @param target Index of the target which must have the component.
        /// @return Component term.
        static QueryTerm makeWithComponent(DataTypeId type, int target);

        /// @brief Returns a new negated component term for the given component and target.
        /// @warning Undefined behaviour will occur if @p type isn't registered as a component.
        /// @param type Component type.
        /// @param target Index of the target which must not have the component.
        /// @return Component term.
        static QueryTerm makeWithoutComponent(DataTypeId type, int target);

        /// @brief Returns a new optional component term for the given component and target.
        /// @warning Undefined behaviour will occur if @p type isn't registered as a component.
        /// @param type Component type.
        /// @param target Index of the target which may have the component.
        /// @return Component term.
        static QueryTerm makeOptComponent(DataTypeId type, int target);

        /// @brief Checks if the term is an entity term.
        /// @return Whether it's an entity term.
        bool isEntity() const;

        /// @brief Checks if the term is a component term.
        /// @param types Types registry.
        /// @return Whether it's a component term.
        bool isComponent(const Types& types) const;

        /// @brief Merges a vector of terms with another, joining pairs of them if possible.
        ///
        /// This function is mainly used to merge terms specified manually for a query with terms obtained from its
        /// argument types. For example, if we have a query of the form `Query<A&, A&>`, we will obtain the terms
        /// `makeWithComponent(A, -1)` and `makeWithComponent(A, -1)`. Those term would be passed into @p other.
        /// If we passed to @p base the terms `makeWithComponent(A, 0)` and `makeWithComponent(A, 1)`, then, the base
        /// wouldn't be modified, and the @p other vector would have its targets updated so that it matches the @p base.
        ///
        /// Any targets found set to -1 will be set to the current default target. The default target is initially 0,
        /// and always changes to the last seen non -1 target. If a relation term has one of the targets undefined, the
        /// default target is incremented.
        ///
        /// Another example would be to have an empty base vector, and the query `Query<A&, B&>`. The @p other vector
        /// would be `{makeWithComponent(A, -1), makeWithComponent(B, -1)}`, and the returned vector would be
        /// `{makeWithComponent(A, 0), makeWithComponent(B, 0)}`.
        ///
        /// @param types Type registry.
        /// @param baseTerms Base vector of terms.
        /// @param[inout] otherTerms Vector of terms to merge into the base. Targets will be modified, if set to -1.
        /// @return Result of merging the two vectors.
        static std::vector<QueryTerm> resolve(const Types& types, const std::vector<QueryTerm>& baseTerms,
                                              std::vector<QueryTerm>& otherTerms);
    };
} // namespace cubos::core::ecs
