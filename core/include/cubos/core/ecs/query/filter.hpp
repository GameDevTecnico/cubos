/// @file
/// @brief Class @ref cubos::core::ecs::QueryFilter.
/// @ingroup core-ecs-query

#pragma once

#include <vector>

#include <cubos/core/ecs/entity/archetype_id.hpp>
#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/query/node/node.hpp>
#include <cubos/core/ecs/query/term.hpp>

namespace cubos::core::ecs
{
    /// @brief Used to find matches for the given query terms. Essentially contains the non-templated part of the query
    /// logic.
    /// @ingroup core-ecs-query
    class QueryFilter
    {
    public:
        /// @brief Can be iterated to view the query matches.
        class View;

        ~QueryFilter();

        /// @brief Constructs.
        /// @param world World being queried.
        /// @param terms Query terms.
        QueryFilter(World& world, const std::vector<QueryTerm>& terms);

        /// @brief Gets the cursor index for the given term index.
        /// @param termIndex Term index.
        /// @return Cursor index.
        std::size_t cursorIndex(std::size_t termIndex) const;

        /// @brief Fetches any new matching archetypes that have been added since the last call to this function.
        void update();

        /// @brief Returns a view which can be used to iterate over the matches.
        /// @return View.
        View view();

        /// @brief Gets the number of targets.
        /// @return Target count.
        int targetCount() const;

    private:
        World& mWorld;

        /// @brief Cursors fo each of the received terms.
        std::vector<std::size_t> mTermCursors;

        /// @brief Number of targets we have, limited to @ref QueryNode::MaxTargetCount.
        int mTargetCount{1};

        /// @brief Number of nodes we have, limited to @ref QueryNode::MaxCursorCount.
        int mNodeCount{0};

        /// @brief Nodes for each cursor. Order is not guaranteed and may change between calls to @ref update.
        QueryNode* mNodes[QueryNode::MaxCursorCount];

        /// @brief Mask with the targets which are already pinned by previous nodes when the respective node is called.
        ///
        /// Kept in sync with @ref mNodes by @ref update.
        QueryNode::TargetMask mNodePins[QueryNode::MaxCursorCount];
    };

    class QueryFilter::View
    {
    public:
        /// @brief Used to iterate over the query matches.
        class Iterator;

        /// @brief Constructs.
        /// @param filter Query filter.
        /// @param pinMask Mask with the targets which are pinned.
        View(QueryFilter& filter, QueryNode::TargetMask pinMask = 0);

        /// @brief Copy constructs.
        /// @param view Other view.
        View(const View& view) = default;

        /// @brief Copy assigns.
        /// @param view Other view.
        View& operator=(const View& view);

        /// @brief Returns a new view equal to this one but with the given target pinned to the given entity.
        ///
        /// Effectively this filters out all matches where the given target isn't the given entity.
        ///
        /// @param target Target index.
        /// @param entity Entity.
        /// @return View.
        View pin(int target, Entity entity);

        /// @brief Returns an iterator pointing to the first query match.
        /// @return Iterator.
        Iterator begin();

        /// @brief Returns an out of bounds iterator representing the end of the query matches.
        /// @return Iterator.
        Iterator end();

    private:
        QueryFilter& mFilter;

        /// @brief Entity handles for each of the pinned targets.
        Entity mPinEntities[QueryNode::MaxTargetCount];

        /// @brief Mask with the targets which are pinned.
        QueryNode::TargetMask mPinMask{0};

        /// @brief Node whose cursor which should be incremented to advance, or -1 if all targets are pinned.
        int mIncNode{-1};

        /// @brief Whether one of the pinned entities is dead. Causes the view to always be empty.
        bool mDead{false};
    };

    class QueryFilter::View::Iterator
    {
    public:
        /// @brief Output structure of the iterator.
        struct Match
        {
            /// @brief Entities for each target.
            Entity entities[QueryNode::MaxTargetCount];
        };

        /// @brief Constructs.
        /// @param view Query filter view.
        /// @param end Whether the iterator should point to the end of the matches.
        Iterator(View& view, bool end);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same match.
        bool operator==(const Iterator& other) const;

        /// @brief Accesses the match referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Match.
        const Match& operator*() const;

        /// @brief Accesses the match referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Match.
        const Match* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

        /// @brief Gets the archetype of each target.
        /// @return Target archetypes array.
        const ArchetypeId* targetArchetypes() const;

        /// @brief Gets the row number for each cursor.
        /// @return Cursor row number array.
        const std::size_t* cursorRows() const;

        /// @brief Checks if the iterator is out of bounds.
        /// @return Whether the iterator is valid.
        bool valid() const;

    private:
        /// @brief Advances the iterator, if necessary, until a match a found.
        void next();

        View& mView;
        mutable Match mMatch;

        int mNodeIndex{0};
        QueryNode::Iterator mIterator;
    };
} // namespace cubos::core::ecs
