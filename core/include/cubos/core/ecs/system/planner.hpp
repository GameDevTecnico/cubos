/// @file
/// @brief Class @ref cubos::core::ecs::Planner.
/// @ingroup core-ecs-system

#pragma once

#include <vector>

#include <cubos/core/ecs/system/schedule.hpp>
#include <cubos/core/ecs/system/tag.hpp>

namespace cubos::core::ecs
{
    /// @brief Layer on top of @ref Schedule, used to build them with added abstractions such as tags.
    class Planner
    {
    public:
        /// @brief Local identifier for tags.
        ///
        /// Differs from the @ref Tag type in that this should only be used internally and can refer to systems.
        struct TagId
        {
            std::size_t inner; ///< Tag index.

            /// @brief Compares two identifiers for equality.
            /// @param other Other identifier.
            /// @return Whether the two identifiers are equal.
            bool operator==(const TagId& other) const = default;
        };

        /// @brief Adds a new tag to the planner.
        /// @return Tag identifier.
        TagId add();

        /// @brief Adds a new system to the planner.
        /// @param systemId System identifier.
        /// @return Tag identifier which includes only the system itself.
        TagId add(SystemId systemId);

        /// @brief Removes a tag from the planner and any constraints related to it.
        ///
        /// Any directly associated systems and conditions are also removed.
        ///
        /// @param tagId Tag identifier.
        void remove(TagId tagId);

        /// @brief Makes the given tag a child of the the given tag.
        ///
        /// Essentially makes the child inherit all constraints of the parent. Tags may have multiple parents, but only
        /// one of them must be a repeated tag.
        ///
        /// Additionally, the tag identified by @p parentId must not be a leaf tag. I.e, it must not have been created
        /// through @ref add(SystemId).
        ///
        /// Tags must not form parent-child cycles.
        ///
        /// @param childId Child tag identifier.
        /// @param parentId Parent tag identifier.
        /// @return Whether the operation was successful.
        bool tag(TagId childId, TagId parentId);

        /// @brief Specifies that the given tag must run before another tag.
        /// @param beforeId Before tag identifier.
        /// @param afterId After tag identifier.
        void order(TagId beforeId, TagId afterId);

        /// @brief Specifies that systems within the given tag should only run if the given condition evaluates to true.
        ///
        /// The condition itself inherits all of the constraints of the tag.
        ///
        /// @param tagId Tag identifier.
        /// @param conditionId Condition identifier.
        void onlyIf(TagId tagId, ConditionId conditionId);

        /// @brief Specifies that the given tag is a repeating tag. Can only be specified once per tag.
        /// @param tagId Tag identifier.
        /// @param conditionId Condition identifier.
        /// @return Whether the operation was successful.
        bool repeatWhile(TagId tagId, ConditionId conditionId);

        /// @brief Constructs a new schedule from the constraints specified until now.
        ///
        /// This operation may fail if a cyclic dependency was specified.
        ///
        /// @return Schedule, or nothing on failure.
        Opt<Schedule> build() const;
    };
} // namespace cubos::core::ecs
