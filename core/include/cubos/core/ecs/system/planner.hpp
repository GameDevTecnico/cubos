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
    class CUBOS_CORE_API Planner
    {
    public:
        /// @brief Local identifier for tags.
        ///
        /// Differs from the @ref Tag type in that this should only be used internally and can refer to systems.
        struct CUBOS_CORE_API TagId
        {
            std::size_t inner; ///< Tag index.

            /// @brief Compares two identifiers for equality.
            /// @param other Other identifier.
            /// @return Whether the two identifiers are equal.
            bool operator==(const TagId& other) const = default;

            struct Hash
            {
                std::size_t operator()(const TagId& tagId) const;
            };
        };

        /// @brief Resets the planner to its initial state.
        void clear();

        /// @brief Adds a new unnamed tag to the planner.
        /// @return Tag identifier.
        TagId add();

        /// @brief Adds a new named tag to the planner.
        /// @param name Debug name.
        /// @return Tag identifier.
        TagId add(std::string name);

        /// @brief Adds a new system to the planner.
        /// @param name Debug name.
        /// @param systemId System identifier.
        /// @return Tag identifier which includes only the system itself.
        TagId add(std::string name, SystemId systemId);

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
        /// Returns false if the tag identified by @p parentId is a leaf tag (i.e., created through @ref add(SystemId)).
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
        ///
        /// Returns false if the given tag was already a repeating tag.
        ///
        /// @param tagId Tag identifier.
        /// @param conditionId Condition identifier.
        /// @return Whether the operation was successful.
        bool repeatWhile(TagId tagId, ConditionId conditionId);

        /// @brief Constructs a new schedule from the constraints specified until now.
        /// @return Schedule, or nothing if the constraints couldn't be fulfilled.
        Opt<Schedule> build() const;

    private:
        /// @brief Holds the constraints and data of a tag.
        struct TagData
        {
            /// @brief Tag debug name.
            std::string name{};

            /// @brief System identifier associated to the tag.
            ///
            /// Only used by leaf tags.
            Opt<SystemId> systemId{};

            /// @brief Condition identifiers associated to the tag.
            std::vector<ConditionId> conditionIds{};

            /// @brief If the tag is a repeat tag, holds its repeat condition.
            Opt<ConditionId> repeatConditionId{};

            /// @brief Parent repeating tag, if there's one. Set internally during @ref build.
            Opt<TagId> repeatingParent{};

            /// @brief Parent tags of this tag.
            std::unordered_set<TagId, TagId::Hash> parents{};

            /// @brief Children tags of this tag.
            std::unordered_set<TagId, TagId::Hash> children{};

            /// @brief Tags which have ordering constraints which force them to run after this tag.
            std::unordered_set<TagId, TagId::Hash> after{};
        };

        /// @brief Checks if a tag is a part of a repeating tag, directly or indirectly.
        /// @param tagId Tag identifier.
        /// @param repeatingId Repeating tag identifier.
        /// @param tags Tags array.
        static bool repeatsWithin(TagId tagId, TagId repeatingId, const std::vector<TagData>& tags);

        /// @brief Recurses into the parent tags, and then copies all of its constraints to the given tag.
        /// @param tagId Tag identifier.
        /// @param tags Tags array.
        /// @param visited Whether each tag has already been collapsed.
        /// @return Whether the tag and its constraints are satisfiable.
        static bool collapse(TagId tagId, std::vector<TagData>& tags, std::vector<bool>& visited);

        /// @brief Expands the children and after fields of all tags.
        /// @param tagId Tag identifier.
        /// @param tags Tags array.
        /// @param visited Whether each tag has already been expanded.
        static void expand(TagId tagId, std::vector<TagData>& tags, std::vector<bool>& visited);

        /// @brief For each after constraint of the given tag, add the missing constraints necessary to satisfy them
        /// even if all non-system and non-repeating tags are removed.
        /// @param tagId Tag identifier.
        /// @param tags Tags array.
        /// @param visited Whether each tag has already been visited.
        static void addMissingOrdering(TagId tagId, std::vector<TagData>& tags, std::vector<bool>& visited);

        /// @brief Creates the system or repeat node for the tag, if it needs one. Recurses into parent repeating tag.
        /// @param tagId Tag identifier.
        /// @param schedule Schedule to add the node to.
        /// @param tags Tags array.
        /// @param nodes Node identifiers for each tag.
        static void makeSystemOrRepeatNode(TagId tagId, Schedule& schedule, const std::vector<TagData>& tags,
                                           std::vector<Opt<Schedule::NodeId>>& nodes);

        std::vector<TagData> mTags{};
    };
} // namespace cubos::core::ecs
