#include <cubos/core/ecs/query/filter.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::QueryFilter;

QueryFilter::QueryFilter(World& world, const std::vector<QueryTerm>& terms)
    : mWorld{world}
    , mTerms{terms}
{
    CUBOS_DEBUG("Creating query filter with the following terms: {}", QueryTerm::toString(world.types(), terms));

    // Count how many targets we have.
    for (const auto& term : terms)
    {
        if (term.isEntity())
        {
            mTargetCount = std::max(mTargetCount, term.entity.target + 1);
        }
        else if (term.isComponent(world.types()))
        {
            mTargetCount = std::max(mTargetCount, term.component.target + 1);
        }
        else if (term.isRelation(world.types()))
        {
            mTargetCount = std::max(mTargetCount, term.relation.fromTarget + 1);
            mTargetCount = std::max(mTargetCount, term.relation.toTarget + 1);
        }
        else
        {
            CUBOS_UNREACHABLE();
        }
    }

    // Make sure we can handle them.
    CUBOS_ASSERT(mTargetCount <= MaxTargetCount, "Currently only targets from 0 to {} are supported",
                 MaxTargetCount - 1);

    // Assign a cursor to each term. The first mTargetCount cursors are used for the targets. The rest are used for
    // relations and are assigned in the order they appear in the terms.
    for (const auto& term : terms)
    {
        if (term.isEntity())
        {
            mTermCursors.emplace_back(term.entity.target);
        }
        else if (term.isComponent(world.types()))
        {
            mTermCursors.emplace_back(term.component.target);
        }
        else if (term.isRelation(world.types()))
        {
            CUBOS_ASSERT(mLinkCount < MaxLinkCount, "Currently only {} links are supported", MaxLinkCount);
            mTermCursors.emplace_back(mTargetCount + mLinkCount);
            mLinks[mLinkCount].dataType = term.type;
            mLinks[mLinkCount].isSymmetric = world.types().isSymmetricRelation(term.type);
            mLinks[mLinkCount].traversal = term.relation.traversal;
            mLinks[mLinkCount].fromTarget = term.relation.fromTarget;
            mLinks[mLinkCount].toTarget = term.relation.toTarget;
            if (world.types().isTreeRelation(term.type))
            {
            }
            ++mLinkCount;
        }
        else
        {
            CUBOS_UNREACHABLE();
        }
    }

    if (mLinkCount == 0)
    {
        CUBOS_ASSERT(mTargetCount == 1, "Currently queries with no links must have exactly one target");
    }

    // Find the base archetypes for each target.
    for (const auto& term : terms)
    {
        // Filter out non 'with component' terms.
        if (!term.isComponent(world.types()) || term.component.optional || term.component.without)
        {
            continue;
        }

        auto& baseArchetype = mTargets[term.component.target].baseArchetype;
        auto columnId = ColumnId::make(term.type);

        if (!world.archetypeGraph().contains(baseArchetype, columnId))
        {
            baseArchetype = world.archetypeGraph().with(baseArchetype, columnId);
        }
    }

    this->update();
}

std::size_t QueryFilter::cursorIndex(std::size_t termIndex) const
{
    CUBOS_ASSERT(termIndex < mTermCursors.size());
    return mTermCursors[termIndex];
}

void QueryFilter::update()
{
    for (int targetIndex = 0; targetIndex < mTargetCount; ++targetIndex)
    {
        auto& target = mTargets[targetIndex];

        // Collect all archetypes which have at least the required components.
        std::vector<ArchetypeId> newArchetypes;
        target.seenCount = mWorld.archetypeGraph().collect(target.baseArchetype, newArchetypes, target.seenCount);
        target.archetypes.reserve(target.archetypes.size() + newArchetypes.size());

        // Add only the archetypes which do not have any of the excluded components.
        for (auto archetype : newArchetypes)
        {
            bool matches = true;

            for (const auto& term : mTerms)
            {
                // Filter out non 'without component' terms and terms with different targets.
                if (!term.isComponent(mWorld.types()) || !term.component.without ||
                    term.component.target != targetIndex)
                {
                    continue;
                }

                auto columnId = ColumnId::make(term.type);
                if (mWorld.archetypeGraph().contains(archetype, columnId))
                {
                    matches = false;
                    break;
                }
            }

            if (matches)
            {
                target.archetypes.emplace_back(archetype);
            }
        }
    }

    // We must check this only after updating the archetypes list, otherwise we might miss some new tables which match
    // new archetypes.
    for (int linkIndex = 0; linkIndex < mLinkCount; ++linkIndex)
    {
        auto& link = mLinks[linkIndex];
        auto prevSeenCount = link.seenCount;

        // Collect all tables which match any of the target archetypes.
        link.seenCount = mWorld.tables().sparseRelation().forEach(link.seenCount, [&](SparseRelationTableId id) {
            if (id.dataType != link.dataType)
            {
                return;
            }

            // The from archetype must be one of the from target archetypes.
            // If the link is symmetric, we also check if the to archetype is one of the to from archetypes.
            bool normalCandidate = false;
            bool reverseCandidate = false;
            for (const auto& archetype : mTargets[link.fromTarget].archetypes)
            {
                if (id.from == archetype)
                {
                    normalCandidate = true;

                    if (!link.isSymmetric || reverseCandidate)
                    {
                        break;
                    }
                }

                if (link.isSymmetric && id.to == archetype)
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
            for (const auto& archetype : mTargets[link.toTarget].archetypes)
            {
                if (id.to == archetype && !normalFound)
                {
                    normalFound = true;
                    link.tables.emplace_back(id);

                    if (reverseFound)
                    {
                        if (!link.reverseTablesSeen.empty())
                        {
                            link.reverseTablesSeen.back() = true;
                        }

                        return;
                    }
                }

                if (id.from == archetype && !reverseFound)
                {
                    reverseFound = true;
                    link.reverseTables.emplace_back(id);
                    link.reverseTablesSeen.emplace_back(normalCandidate && normalFound);

                    if (normalFound)
                    {
                        return;
                    }
                }
            }
        });

        if (!link.isSymmetric)
        {
            CUBOS_ASSERT(link.reverseTables.empty());
        }

        if (prevSeenCount != link.seenCount)
        {
            if (link.traversal == Traversal::Up)
            {
                CUBOS_ASSERT(!link.isSymmetric);
                std::sort(
                    link.tables.begin(), link.tables.end(),
                    [](const SparseRelationTableId& a, const SparseRelationTableId& b) { return a.depth > b.depth; });
            }
            else if (link.traversal == Traversal::Down)
            {
                CUBOS_ASSERT(!link.isSymmetric);
                std::sort(
                    link.tables.begin(), link.tables.end(),
                    [](const SparseRelationTableId& a, const SparseRelationTableId& b) { return a.depth < b.depth; });
            }
        }
    }
}

auto QueryFilter::view() -> View
{
    return {*this};
}

int QueryFilter::targetCount() const
{
    return mTargetCount;
}

auto QueryFilter::Link::table(std::size_t index) const -> SparseRelationTableId
{
    if (index < tables.size())
    {
        return tables[index];
    }

    return reverseTables[index - tables.size()];
}

QueryFilter::View::View(QueryFilter& filter)
    : mFilter{filter}
{
}

auto QueryFilter::View::operator=(const View& view) -> View&
{
    CUBOS_ASSERT(&mFilter == &view.mFilter);

    for (int target = 0; target < view.mFilter.mTargetCount; ++target)
    {
        mPins[target] = view.mPins[target];
    }

    return *this;
}

auto QueryFilter::View::pin(int target, Entity entity) -> View
{
    CUBOS_ASSERT(target < mFilter.mTargetCount, "No such target {} in query", target);
    CUBOS_ASSERT(mPins[target].isNull(), "Target {} was already pinned", target);

    View view{mFilter};

    // If the entity is dead, set the view as dead, to guarantee that the iterator will not return any matches.
    view.mDead = mDead;
    if (!mFilter.mWorld.isAlive(entity))
    {
        view.mDead = true;
    }

    // Pass the pins from the current view to the new view.
    for (int i = 0; i < mFilter.mTargetCount; ++i)
    {
        view.mPins[i] = mPins[i];
    }
    view.mPins[target] = entity;
    return view;
}

auto QueryFilter::View::begin() -> Iterator
{
    return {*this, mDead};
}

auto QueryFilter::View::end() -> Iterator
{
    return {*this, true};
}

QueryFilter::View::Iterator::Iterator(View& view, bool end)
    : mView{view}
{
    // Initially always point to the end.
    mIndex = this->endIndex();
    for (int i = 0; i < mView.mFilter.mTargetCount + mView.mFilter.mLinkCount; ++i)
    {
        mCursorRows[i] = 0;
    }

    // If end is false, then we wrap around to the first match.
    if (!end)
    {
        this->advance();
    }
}

bool QueryFilter::View::Iterator::operator==(const Iterator& other) const
{
    if (&mView != &other.mView || mIndex != other.mIndex)
    {
        return false;
    }

    for (int cursor = 0; cursor < mView.mFilter.mTargetCount + mView.mFilter.mLinkCount; ++cursor)
    {
        if (mCursorRows[cursor] != other.mCursorRows[cursor])
        {
            return false;
        }
    }

    return true;
}

auto QueryFilter::View::Iterator::operator*() const -> const Match&
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");

    auto& world = mView.mFilter.mWorld;

    for (int i = 0; i < mView.mFilter.mTargetCount; ++i)
    {
        mMatch.entities[i].index = world.tables().dense().at(mTargetArchetypes[i]).entity(mCursorRows[i]);
        mMatch.entities[i].generation = world.generation(mMatch.entities[i].index);
    }

    return mMatch;
}

auto QueryFilter::View::Iterator::operator->() const -> const Match*
{
    return &this->operator*();
}

auto QueryFilter::View::Iterator::operator++() -> Iterator&
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");
    this->advance();
    return *this;
}

const ArchetypeId* QueryFilter::View::Iterator::targetArchetypes() const
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");
    return mTargetArchetypes;
}

const std::size_t* QueryFilter::View::Iterator::cursorRows() const
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");
    return mCursorRows;
}

bool QueryFilter::View::Iterator::valid() const
{
    return mIndex < this->endIndex();
}

void QueryFilter::View::Iterator::advance()
{
    auto& filter = mView.mFilter;
    auto& world = filter.mWorld;

    if (filter.mLinkCount == 0)
    {
        // Basic case where we have no links and a single target.
        auto& archetypes = filter.mTargets[0].archetypes;

        // Check if the single target is pinned.
        if (mView.mPins[0].isNull())
        {
            // No pins, just advance to the next cached archetype.
            if (mIndex == archetypes.size())
            {
                mIndex = 0; // Wrap around if we reached the end.
            }
            else
            {
                ++mCursorRows[0]; // Advance to the next row in the current archetype.
            }

            // Find the next cached archetype which has a dense table (and thus at least one entity).
            // If the cursor row is still in bounds of the current archetype, we stay in it.
            while (mIndex < archetypes.size() &&
                   (!world.tables().dense().contains(archetypes[mIndex]) ||
                    mCursorRows[0] >= world.tables().dense().at(archetypes[mIndex]).size()))
            {
                ++mIndex;
                mCursorRows[0] = 0;
            }

            // Update the target archetype.
            if (mIndex < archetypes.size())
            {
                mTargetArchetypes[0] = archetypes[mIndex];
            }
        }
        else
        {
            mCursorRows[0] = 0;

            // If it is pinned, we either move the iterator to the pinned entity or to the end.
            if (mIndex != archetypes.size())
            {
                mIndex = archetypes.size();
            }
            else
            {
                // Find the pinned entity's archetype index.
                auto entity = mView.mPins[0];
                auto archetype = world.archetype(entity);
                mIndex = 0;
                while (mIndex < archetypes.size() && archetypes[mIndex] != archetype)
                {
                    ++mIndex;
                }

                if (mIndex < archetypes.size())
                {
                    mCursorRows[0] = world.tables().dense().at(archetype).row(entity.index);
                    mTargetArchetypes[0] = archetype;
                }
            }
        }
    }
    else
    {
        // We have one link and two targets.
        auto& link = filter.mLinks[0];

        if (link.tables.empty() && link.reverseTables.empty())
        {
            // If no table matches the query, we can't advance.
            return;
        }

        // Check if any of the targets is pinned.
        if (mView.mPins[link.fromTarget].isNull() && mView.mPins[link.toTarget].isNull())
        {
            // No pins, just advance to the next cached table.
            if (mIndex == link.tables.size() + link.reverseTables.size())
            {
                mIndex = 0; // Wrap around if we reached the end.
            }
            else
            {
                ++mCursorRows[filter.mTargetCount]; // Advance to the next row in the current table.
            }

            // Find the next cached table where our cursor is in bounds, i.e., where there are still rows to iterate
            // over. If the cursor row is still in bounds of the current table, we stay in it.
            while (mIndex < link.tables.size() + link.reverseTables.size() &&
                   (mCursorRows[filter.mTargetCount] >= world.tables().sparseRelation().at(link.table(mIndex)).size() ||
                    (mIndex >= link.tables.size() && link.reverseTablesSeen[mIndex - link.tables.size()])))
            {
                ++mIndex;
                mCursorRows[filter.mTargetCount] = 0;
            }
        }
        else if (mView.mPins[link.fromTarget].isNull())
        {
            auto toEntity = mView.mPins[link.toTarget];

            // The to target is pinned.
            if (mIndex == link.tables.size() + link.reverseTables.size())
            {
                mIndex = 0; // Wrap around if we reached the end.
                auto& table = world.tables().sparseRelation().at(link.table(mIndex));
                if (mIndex < link.tables.size())
                {
                    mCursorRows[filter.mTargetCount] = table.firstTo(toEntity.index);
                }
                else
                {
                    mCursorRows[filter.mTargetCount] = table.firstFrom(toEntity.index);
                }
            }
            else
            {
                // Find the next row in the relation table which matches the pinned entity.
                auto& table = world.tables().sparseRelation().at(link.table(mIndex));
                if (mIndex < link.tables.size())
                {
                    mCursorRows[filter.mTargetCount] = table.nextTo(mCursorRows[filter.mTargetCount]);
                }
                else
                {
                    mCursorRows[filter.mTargetCount] = table.nextFrom(mCursorRows[filter.mTargetCount]);
                }
            }

            // Advance to the next cached table as long as the cursor is out of bounds.
            while (mIndex < link.tables.size() + link.reverseTables.size() &&
                   mCursorRows[filter.mTargetCount] >= world.tables().sparseRelation().at(link.table(mIndex)).size())
            {
                ++mIndex;

                if (mIndex < link.tables.size() + link.reverseTables.size())
                {
                    // Jump to the first row which matches the pinned entity.
                    auto& table = world.tables().sparseRelation().at(link.table(mIndex));
                    if (mIndex < link.tables.size())
                    {
                        mCursorRows[filter.mTargetCount] = table.firstTo(toEntity.index);
                    }
                    else
                    {
                        mCursorRows[filter.mTargetCount] = table.firstFrom(toEntity.index);
                    }
                }
            }
        }
        else if (mView.mPins[link.toTarget].isNull())
        {
            auto fromEntity = mView.mPins[link.fromTarget];

            // The from target is pinned.
            if (mIndex == link.tables.size() + link.reverseTables.size())
            {
                mIndex = 0; // Wrap around if we reached the end.
                auto& table = world.tables().sparseRelation().at(link.table(mIndex));
                if (mIndex < link.tables.size())
                {
                    mCursorRows[filter.mTargetCount] = table.firstFrom(fromEntity.index);
                }
                else
                {
                    mCursorRows[filter.mTargetCount] = table.firstTo(fromEntity.index);
                }
            }
            else
            {
                // Find the next row in the relation table which matches the pinned entity.
                auto& table = world.tables().sparseRelation().at(link.table(mIndex));
                if (mIndex < link.tables.size())
                {
                    mCursorRows[filter.mTargetCount] = table.nextFrom(mCursorRows[filter.mTargetCount]);
                }
                else
                {
                    mCursorRows[filter.mTargetCount] = table.nextTo(mCursorRows[filter.mTargetCount]);
                }
            }

            // Advance to the next cached table as long as the cursor is out of bounds.
            while (mIndex < link.tables.size() + link.reverseTables.size() &&
                   mCursorRows[filter.mTargetCount] >= world.tables().sparseRelation().at(link.table(mIndex)).size())
            {
                ++mIndex;

                if (mIndex < link.tables.size() + link.reverseTables.size())
                {
                    // Jump to the first row which matches the pinned entity.
                    auto& table = world.tables().sparseRelation().at(link.table(mIndex));
                    if (mIndex < link.tables.size())
                    {
                        mCursorRows[filter.mTargetCount] = table.firstFrom(fromEntity.index);
                    }
                    else
                    {
                        mCursorRows[filter.mTargetCount] = table.firstTo(fromEntity.index);
                    }
                }
            }
        }
        else
        {
            // Both targets are pinned, so we either move the iterator to the pinned entities or to the end.
            if (mIndex != link.tables.size() + link.reverseTables.size())
            {
                mIndex = link.tables.size() + link.reverseTables.size();
            }
            else
            {
                auto fromEntity = mView.mPins[link.fromTarget];
                auto toEntity = mView.mPins[link.toTarget];

                auto fromArchetype = world.archetype(fromEntity);
                auto toArchetype = world.archetype(toEntity);

                // Find the index of the table which matches the pinned entities.
                mIndex = 0;
                while (mIndex < link.tables.size() &&
                       (link.table(mIndex).from != fromArchetype || link.table(mIndex).to != toArchetype))
                {
                    ++mIndex;
                }

                if (fromArchetype == toArchetype && fromEntity.index > toEntity.index)
                {
                    // We have to search for the table in the reversed tables list.
                    mIndex = link.tables.size();
                }

                if (mIndex == link.tables.size())
                {
                    // Try the reverse tables.
                    while (mIndex < link.tables.size() + link.reverseTables.size() &&
                           (link.table(mIndex).from != toArchetype || link.table(mIndex).to != fromArchetype))
                    {
                        ++mIndex;
                    }
                }

                if (mIndex < link.tables.size() + link.reverseTables.size())
                {
                    // Find the row of the pinned entities in the table.
                    auto& table = world.tables().sparseRelation().at(link.table(mIndex));

                    if (mIndex < link.tables.size())
                    {
                        mCursorRows[filter.mTargetCount] = table.row(fromEntity.index, toEntity.index);
                    }
                    else
                    {
                        mCursorRows[filter.mTargetCount] = table.row(toEntity.index, fromEntity.index);
                    }

                    if (mCursorRows[filter.mTargetCount] == table.size())
                    {
                        // Turns out the entities are not related after all.
                        mIndex = link.tables.size() + link.reverseTables.size();
                    }
                }
            }
        }

        // Update the target archetypes.
        if (mIndex < link.tables.size())
        {
            mTargetArchetypes[link.fromTarget] = link.table(mIndex).from;
            mTargetArchetypes[link.toTarget] = link.table(mIndex).to;

            // Get the entity indices of the current row.
            uint32_t fromIndex = UINT32_MAX;
            uint32_t toIndex = UINT32_MAX;
            world.tables()
                .sparseRelation()
                .at(link.table(mIndex))
                .indices(mCursorRows[filter.mTargetCount], fromIndex, toIndex);

            // Get the rows of the entities in their respective dense tables.
            mCursorRows[link.fromTarget] = world.tables().dense().at(link.table(mIndex).from).row(fromIndex);
            mCursorRows[link.toTarget] = world.tables().dense().at(link.table(mIndex).to).row(toIndex);
        }
        else if (mIndex < link.tables.size() + link.reverseTables.size())
        {
            // We're iterating over the reverse tables.
            mTargetArchetypes[link.fromTarget] = link.table(mIndex).to;
            mTargetArchetypes[link.toTarget] = link.table(mIndex).from;

            // Get the entity indices of the current row.
            uint32_t fromIndex = UINT32_MAX;
            uint32_t toIndex = UINT32_MAX;
            world.tables()
                .sparseRelation()
                .at(link.table(mIndex))
                .indices(mCursorRows[filter.mTargetCount], fromIndex, toIndex);

            if (link.table(mIndex).from == link.table(mIndex).to && fromIndex == toIndex)
            {
                // We have already visited these relations on the non-reversed tables.
                this->advance();
            }
            else
            {
                // Get the rows of the entities in their respective dense tables.
                mCursorRows[link.fromTarget] = world.tables().dense().at(link.table(mIndex).to).row(toIndex);
                mCursorRows[link.toTarget] = world.tables().dense().at(link.table(mIndex).from).row(fromIndex);
            }
        }
        else
        {
            mCursorRows[filter.mTargetCount] = 0;
            mCursorRows[link.fromTarget] = 0;
            mCursorRows[link.toTarget] = 0;
        }
    }
}

std::size_t QueryFilter::View::Iterator::endIndex() const
{
    if (mView.mFilter.mLinkCount == 0)
    {
        return mView.mFilter.mTargets[0].archetypes.size();
    }

    auto& link = mView.mFilter.mLinks[0];
    return link.tables.size() + link.reverseTables.size();
}
