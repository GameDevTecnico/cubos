/// @file
/// @brief Class @ref cubos::core::ecs::QueryFilter.
/// @ingroup core-ecs-query

#pragma once

#include <vector>

#include <cubos/core/ecs/entity/archetype_id.hpp>
#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/query/term.hpp>
#include <cubos/core/ecs/table/sparse_relation/id.hpp>

namespace cubos::core::ecs
{
    class World;

    /// @brief Used to find matches for the given query terms. Essentially contains the non-templated part of the query
    /// logic.
    /// @ingroup core-ecs-query
    class QueryFilter
    {
    public:
        /// @brief Maximum number of targets per match allowed in a single query.
        static constexpr int MaxTargetCount = 2;

        /// @brief Maximum number of links per query.
        static constexpr int MaxLinkCount = 1;

        /// @brief Can be iterated to view the query matches.
        class View;

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
        /// @brief Holds the necessary data for each target.
        struct Target
        {
            /// @brief Archetype which holds only the required components for this target.
            ArchetypeId baseArchetype{ArchetypeId::Empty};

            /// @brief Archetypes which match the target, found through calls to @ref update().
            std::vector<ArchetypeId> archetypes;

            /// @brief How many archetypes have already been seen through @ref ArchetypeGraph::collect.
            std::size_t seenCount{0};
        };

        /// @brief Holds the necessary data for each link.
        struct Link
        {
            /// @brief Relation data type.
            DataTypeId dataType;

            /// @brief Whether the link is symmetric.
            bool isSymmetric;

            /// @brief Link traversal type.
            Traversal traversal;

            /// @brief From target index.
            int fromTarget;

            /// @brief To target index.
            int toTarget;

            /// @brief Tables which match the link, found through calls to @ref update().
            std::vector<SparseRelationTableId> tables;

            /// @brief Tables which match the reverse link, found through calls to @ref update().
            ///
            /// Only filled if @ref isSymmetric is true.
            std::vector<SparseRelationTableId> reverseTables;

            /// @brief Whether each of the tables in @ref reverseTables is already in @ref tables.
            ///
            /// Only filled if @ref isSymmetric is true.
            std::vector<bool> reverseTablesSeen;

            /// @brief How many tables have already been seen through @ref SparseRelationTableRegistry::collect.
            std::size_t seenCount{0};

            /// @brief Gets the nth table in the link, as if the reverseTables vector was appended to the end of the
            /// tables vector.
            /// @param index Table index.
            SparseRelationTableId table(std::size_t index) const;
        };

        World& mWorld;

        std::vector<QueryTerm> mTerms;
        std::vector<std::size_t> mTermCursors;

        Target mTargets[MaxTargetCount];
        int mTargetCount{1};
        Link mLinks[MaxLinkCount];
        int mLinkCount{0};
    };

    class QueryFilter::View
    {
    public:
        /// @brief Used to iterate over the query matches.
        class Iterator;

        /// @brief Constructs.
        /// @param filter Query filter.
        View(QueryFilter& filter);

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
        Entity mPins[MaxTargetCount];
        bool mDead{false};
    };

    class QueryFilter::View::Iterator
    {
    public:
        /// @brief Output structure of the iterator.
        struct Match
        {
            /// @brief Entities for each target.
            Entity entities[MaxTargetCount];
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
        /// @brief Advances the iterator to the next valid match. Wraps around.
        void advance();

        /// @brief Gets the index value which represents the end of the iteration.
        /// @return End index.
        std::size_t endIndex() const;

        View& mView;
        mutable Match mMatch;

        /// @brief Archetype of each target.
        ArchetypeId mTargetArchetypes[MaxTargetCount];

        /// @brief Index of the currently selected archetype or sparse relation table.
        ///
        /// We distinguish between them through the mLinkCount variable. If it's 0, we're iterating over archetypes.
        /// Otherwise, we're iterating over sparse relation tables.
        std::size_t mIndex;

        /// @brief Row number of each cursor.
        ///
        /// The first mTargetCount cursors represent the targets.
        /// The remaining mLinkCount cursors represent the links.
        std::size_t mCursorRows[MaxTargetCount + MaxLinkCount];
    };
} // namespace cubos::core::ecs
