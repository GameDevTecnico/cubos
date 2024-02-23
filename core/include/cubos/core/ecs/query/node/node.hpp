/// @file
/// @brief Class @ref cubos::core::ecs::QueryNode.
/// @ingroup core-ecs-query

#pragma once

#include <cstddef>

#include <cubos/core/ecs/entity/archetype_id.hpp>

namespace cubos::core::ecs
{
    class World;

    /// @brief Query filter step, which receives an iterator and advances it until it points to a valid match.
    class QueryNode
    {
    public:
        /// @brief Target mask type.
        using TargetMask = unsigned char;

        /// @brief Maximum number of targets.
        static constexpr int MaxTargetCount = sizeof(TargetMask) * 8;

        /// @brief Maximum number of cursors.
        static constexpr int MaxCursorCount = MaxTargetCount;

        /// @brief Points to a specific query match.
        struct Iterator;

        virtual ~QueryNode() = default;

        /// @brief Constructs.
        QueryNode(int cursor);

        /// @brief Gets the cursor index of the node.
        int cursor() const;

        /// @brief Gets the mask which indicates which targets the node pins.
        TargetMask pins() const;

        /// @brief Returns a rough estimate on how many matches this node produces by itself.
        ///
        /// Nodes are sorted by this value to minimize the number of iterations. It makes more sense to first run the
        /// node which produces the fewest matches, as it will be the most selective.
        virtual std::size_t estimate() const = 0;

        /// @brief Updates the node with new data from the given world.
        /// @param world World.
        virtual void update(World& world) = 0;

        /// @brief Advances the iterator to the next valid match, or checks if the pinned targets are valid.
        ///
        /// Operates in two different modes:
        /// - Iterate: at least one of the node targets is not pinned.
        /// - Validate: all node targets are already pinned.
        ///
        /// When iterating, this function should receive an iterator with the cursor index set to SIZE_MAX, or one
        /// previously returned by this function. It will update the iterator's cursor index and row to point to the
        /// first or next valid match. The relevant target archetypes and cursor rows will be set for any newly pinned
        /// targets.
        ///
        /// When validating, the function will only check if the pinned targets match the requirements and return true
        /// or false without modifying the iterator. The passed iterator should have the pinned targets' archetypes and
        /// cursor rows set.
        ///
        /// @param world World being queried.
        /// @param pins Whether each target is pinned.
        /// @param iterator Iterator.
        /// @return Whether a match was found.
        virtual bool next(World& world, TargetMask pins, Iterator& iterator) const = 0;

    protected:
        /// @brief Indicates that the node pins the given target.
        /// @param target Target.
        void pins(int target);

    private:
        /// @brief Cursor index of the node.
        const int mCursor;

        /// @brief Mask which indicates which targets become pinned by this node.
        TargetMask mPins{0};
    };

    struct QueryNode::Iterator
    {
        /// @brief Archetype of each target.
        ArchetypeId targetArchetypes[MaxTargetCount];

        /// @brief Index which identifies the archetype or table of each cursor.
        std::size_t cursorIndex[MaxCursorCount];

        /// @brief Row number of each cursor. The first 'target count' rows are reserved for the target rows.
        std::size_t cursorRows[MaxCursorCount];
    };
} // namespace cubos::core::ecs
