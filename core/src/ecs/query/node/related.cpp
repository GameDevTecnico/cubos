#include <cubos/core/ecs/query/node/related.hpp>
#include <cubos/core/ecs/world.hpp>

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::QueryRelatedNode;

QueryRelatedNode::QueryRelatedNode(int cursor, DataTypeId dataType, bool isSymmetric, bool includeDuplicates,
                                   Traversal traversal, QueryArchetypeNode& fromNode, QueryArchetypeNode& toNode)
    : QueryNode(cursor)
    , mDataType{dataType}
    , mIsSymmetric{isSymmetric}
    , mIncludeDuplicates{includeDuplicates}
    , mTraversal{traversal}
    , mFromNode{fromNode}
    , mToNode{toNode}
{
    this->pins(fromNode.target());
    this->pins(toNode.target());

    CUBOS_ASSERT_IMP(mIsSymmetric, mTraversal == Traversal::Random);
    CUBOS_ASSERT_IMP(mIncludeDuplicates, mIsSymmetric);
}

std::size_t QueryRelatedNode::estimate() const
{
    // TODO: use a better estimate here.
    return 0;
}

void QueryRelatedNode::update(World& world)
{
    // Since we depend on the archetype nodes for maintaining our cache, we must guarantee they are updated first.
    mFromNode.update(world);
    mToNode.update(world);

    auto prevSeenCount = mSeenCount;

    // Collect all tables which match any of the target archetypes.
    mSeenCount = world.tables().sparseRelation().forEach(mSeenCount, [&](SparseRelationTableId id) {
        if (id.dataType != mDataType)
        {
            return;
        }

        // The from archetype must be one of the from target archetypes.
        // If the link is symmetric, we also check if the to archetype is one of the to from archetypes.
        bool normalCandidate = false;
        bool reverseCandidate = false;
        for (const auto& archetype : mFromNode.archetypes())
        {
            if (id.from == archetype)
            {
                normalCandidate = true;

                if (!mIsSymmetric || reverseCandidate)
                {
                    break;
                }
            }

            if (mIsSymmetric && id.to == archetype)
            {
                reverseCandidate = true;

                if (normalCandidate)
                {
                    break;
                }
            }
        }

        // Early exit if no candidate was found.
        if (!normalCandidate && !reverseCandidate)
        {
            return;
        }

        // The to archetype must be one of the to target archetypes.
        // If the link is symmetric, we also check if the from archetype is one of the to target archetypes.
        bool normalFound = !normalCandidate;
        bool reverseFound = !reverseCandidate;
        for (const auto& archetype : mToNode.archetypes())
        {
            if (id.to == archetype && !normalFound)
            {
                normalFound = true;
                mTables.emplace_back(id);

                if (reverseFound)
                {
                    if (reverseCandidate)
                    {
                        mReverseTables.back().isDuplicate = true;
                    }

                    return;
                }
            }

            if (id.from == archetype && !reverseFound)
            {
                reverseFound = true;
                mReverseTables.emplace_back(ReverseEntry{.id = id, .isDuplicate = normalFound && normalCandidate});

                if (normalFound)
                {
                    return;
                }
            }
        }
    });

    CUBOS_ASSERT_IMP(!mIsSymmetric, mReverseTables.empty());

    // If we added new tables to the cache, and a traversal direction was specified, we must sort the cached tables in
    // the right order.
    if (prevSeenCount != mSeenCount)
    {
        if (mTraversal == Traversal::Up)
        {
            std::sort(mTables.begin(), mTables.end(),
                      [](const SparseRelationTableId& a, const SparseRelationTableId& b) { return a.depth > b.depth; });
        }
        else if (mTraversal == Traversal::Down)
        {
            std::sort(mTables.begin(), mTables.end(),
                      [](const SparseRelationTableId& a, const SparseRelationTableId& b) { return a.depth < b.depth; });
        }
    }
}

bool QueryRelatedNode::next(World& world, TargetMask pins, Iterator& iterator) const
{
    bool fromPinned = pins & (1 << mFromNode.target());
    bool toPinned = pins & (1 << mToNode.target());

    // Define some utility aliases.
    auto& tableIndex = iterator.cursorIndex[this->cursor()];
    auto& row = iterator.cursorRows[this->cursor()];
    auto& denseTables = world.tables().dense();
    auto& relationTables = world.tables().sparseRelation();

    if (fromPinned && toPinned)
    {
        // If both targets are pinned, we are in validation mode and we can simply check if the relation exists.
        // Start by finding the archetypes and entity indices of the pinned targets.
        auto fromArchetype = iterator.targetArchetypes[mFromNode.target()];
        auto fromIndex = denseTables.at(fromArchetype).entity(iterator.cursorRows[mFromNode.target()]);
        auto toArchetype = iterator.targetArchetypes[mToNode.target()];
        auto toIndex = denseTables.at(toArchetype).entity(iterator.cursorRows[mToNode.target()]);

        // If the relation is symmetric, make sure we guarantee the symmetry invariant.
        if (mIsSymmetric && (fromArchetype.inner > toArchetype.inner ||
                             (fromArchetype.inner == toArchetype.inner && fromIndex > toIndex)))
        {
            std::swap(fromArchetype, toArchetype);
            std::swap(fromIndex, toIndex);
        }

        // Then search for a table between the two archetypes which may contain the relation.
        auto tableId = SparseRelationTableId{mDataType, fromArchetype, toArchetype};
        for (; tableId.depth <= relationTables.type(mDataType).maxDepth(); ++tableId.depth)
        {
            if (relationTables.contains(tableId))
            {
                auto& table = relationTables.at(tableId);
                row = table.row(fromIndex, toIndex);
                if (row != table.size())
                {
                    return true; // Stop as soon as we find the relation we were looking for.
                }
            }
        }

        return false;
    }
    else if (fromPinned || toPinned)
    {
        // Only one of the targets is pinned, thus we need to find the next valid match for the free target.
        // Start by getting the cursors of the pinned and unpinned targets.
        auto pinnedCursor = fromPinned ? mFromNode.target() : mToNode.target();
        auto unpinnedCursor = fromPinned ? mToNode.target() : mFromNode.target();

        // Find the archetype and entity index of the pinned target.
        auto pinnedArchetype = iterator.targetArchetypes[pinnedCursor];
        auto pinnedIndex = denseTables.at(pinnedArchetype).entity(iterator.cursorRows[pinnedCursor]);

        // Declare some variables which we will be managed by the setTable function.
        SparseRelationTableId tableId;
        SparseRelationTable* table;
        bool isReverse;

        // Utility function which updates the table pointer based on the current table index.
        auto setTable = [&]() {
            if (tableIndex < mTables.size())
            {
                tableId = mTables[tableIndex];
                table = &relationTables.at(tableId);
                isReverse = false;
            }
            else if (tableIndex < mTables.size() + mReverseTables.size())
            {
                tableId = mReverseTables[tableIndex - mTables.size()].id;
                table = &relationTables.at(tableId);
                isReverse = true;
            }
            else
            {
                table = nullptr;
            }
        };

        // Utility function which sets the row to the first valid row of the current table based on the given side.
        auto resetRow = [&]() {
            row = (fromPinned ^ isReverse) ? table->firstFrom(pinnedIndex) : table->firstTo(pinnedIndex);
        };

        // Utility function which advances the row to the next valid row of the current table based on the given side.
        auto advanceRow = [&]() { row = (fromPinned ^ isReverse) ? table->nextFrom(row) : table->nextTo(row); };

        // Utility function which checks if the current row stores a relation from an entity to itself.
        auto skipIfIdentityRow = [&]() {
            if (isReverse && !mIncludeDuplicates && row < table->size() && table->from(row) == table->to(row))
            {
                advanceRow();
            }
        };

        // Get data regarding the table we're currently iterating.
        setTable();
        if (table == nullptr)
        {
            // If there is no table to iterate, try going back to the first table.
            tableIndex = 0;
            setTable();
            if (table == nullptr)
            {
                // There are no tables to iterate at all, return false.
                return false;
            }

            // New table, go to the first matching row.
            resetRow();
        }
        else
        {
            // Advance to the next matching row in the current table.
            advanceRow();
        }

        // Make sure we don't match the same row twice.
        skipIfIdentityRow();

        // While the row is out of bounds, we should advance to the next table.
        while (row >= table->size())
        {
            ++tableIndex;
            setTable();

            if (table == nullptr)
            {
                return false;
            }

            resetRow();
            skipIfIdentityRow();
        }

        // Update the archetype and cursor of the 'unpinned' target.
        auto unpinnedArchetype = (fromPinned ^ isReverse) ? tableId.to : tableId.from;
        uint32_t unpinnedIndex = (fromPinned ^ isReverse) ? table->to(row) : table->from(row);
        iterator.targetArchetypes[unpinnedCursor] = unpinnedArchetype;
        iterator.cursorRows[unpinnedCursor] = denseTables.at(unpinnedArchetype).row(unpinnedIndex);
        return true;
    }

    // If table index is out of bounds, we're starting from the beginning.
    if (tableIndex >= mTables.size() + mReverseTables.size())
    {
        tableIndex = 0;
        row = 0;
    }
    else
    {
        ++row;
    }

    // Find the next cached table where the current row is within bounds.
    while (tableIndex < mTables.size() && row >= world.tables().sparseRelation().at(mTables[tableIndex]).size())
    {
        ++tableIndex;
        row = 0;
    }

    if (tableIndex >= mTables.size())
    {
        // If we've tried all normal tables, try the ones facing the reverse direction.
        // If mIncludeDuplicates is false, skip duplicate tables.
        while (tableIndex < mTables.size() + mReverseTables.size() &&
               (row >= world.tables().sparseRelation().at(mReverseTables[tableIndex - mTables.size()].id).size() ||
                (!mIncludeDuplicates && mReverseTables[tableIndex - mTables.size()].isDuplicate)))
        {
            ++tableIndex;
            row = 0;
        }
    }

    // If we didn't find any more entries, return false.
    if (tableIndex == mTables.size() + mReverseTables.size())
    {
        return false;
    }

    // Get the index of the 'from' and 'to' entities.
    auto id = tableIndex >= mTables.size() ? mReverseTables[tableIndex - mTables.size()].id : mTables[tableIndex];
    uint32_t fromIndex;
    uint32_t toIndex;
    world.tables().sparseRelation().at(id).indices(row, fromIndex, toIndex);
    auto fromArchetype = id.from;
    auto toArchetype = id.to;

    // If we're traversing in the reverse direction, swap the entities.
    if (tableIndex >= mTables.size())
    {
        std::swap(fromArchetype, toArchetype);
        std::swap(fromIndex, toIndex);
    }

    // Update the archetype and cursor of the 'from' and 'to' targets.
    iterator.targetArchetypes[mFromNode.target()] = fromArchetype;
    iterator.targetArchetypes[mToNode.target()] = toArchetype;
    iterator.cursorRows[mFromNode.target()] = world.tables().dense().at(fromArchetype).row(fromIndex);
    iterator.cursorRows[mToNode.target()] = world.tables().dense().at(toArchetype).row(toIndex);
    return true;
}
