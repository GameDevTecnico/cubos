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
            mLinks[mLinkCount].fromTarget = term.relation.fromTarget;
            mLinks[mLinkCount].toTarget = term.relation.toTarget;
            ++mLinkCount;
        }
        else
        {
            CUBOS_UNREACHABLE();
        }
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
                // Filter out non 'without component' terms.
                if (!term.isComponent(mWorld.types()) || !term.component.without)
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

        // Collect all tables which match any of the target archetypes.
        link.seenCount =
            mWorld.tables().sparseRelation().collect(link.tables, link.seenCount, [&](SparseRelationTableId id) {
                // The from archetype must be one of the from target archetypes.
                bool found = false;
                for (const auto& archetype : mTargets[link.fromTarget].archetypes)
                {
                    if (id.from == archetype)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    return false;
                }

                // The to archetype must be one of the to target archetypes.
                for (const auto& archetype : mTargets[link.toTarget].archetypes)
                {
                    if (id.to == archetype)
                    {
                        return true;
                    }
                }

                return false;
            });
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

auto QueryFilter::View::pin(int target, Entity entity) && -> View
{
    CUBOS_ASSERT(target < mFilter.mTargetCount, "No such target {} in query", target);
    CUBOS_ASSERT(mPins[target].isNull(), "Target {} was already pinned", target);
    mPins[target] = entity;
    return *this;
}

auto QueryFilter::View::begin() -> Iterator
{
    return {*this, false};
}

auto QueryFilter::View::end() -> Iterator
{
    return {*this, true};
}

QueryFilter::View::Iterator::Iterator(View& view, bool end)
    : mView{view}
{
    mIndex = 0;
    for (int i = 0; i < mView.mFilter.mTargetCount + mView.mFilter.mLinkCount; ++i)
    {
        mCursorRows[i] = 0;
    }

    if (end)
    {
        mIndex = this->endIndex();
    }
    else
    {
        // Set the archetype and cursor row for each of the pinned targets.
        for (int target = 0; target < mView.mFilter.mTargetCount; ++target)
        {
            auto pin = mView.mPins[target];
            if (!pin.isNull())
            {
                if (target == 0 && mView.mFilter.mLinkCount == 0)
                {
                    mIndex = mView.mFilter.mTargets[target].archetypes.size();
                }

                // Find archetype index.
                auto archetype = mView.mFilter.mWorld.archetype(pin);
                for (std::size_t i = 0; i < mView.mFilter.mTargets[target].archetypes.size(); ++i)
                {
                    if (mView.mFilter.mTargets[target].archetypes[i] == archetype)
                    {
                        if (target == 0 && mView.mFilter.mLinkCount == 0)
                        {
                            mIndex = i;
                        }

                        mCursorRows[target] = mView.mFilter.mWorld.tables().dense().at(archetype).row(pin.index);
                        break;
                    }
                }
            }
        }
    }

    this->findArchetype();
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
    if (mView.mFilter.mLinkCount > 0)
    {
        CUBOS_ASSERT(mIndex < mView.mFilter.mLinks[0].tables.size(), "Iterator out of bounds");
    }
    else
    {
        CUBOS_ASSERT(mIndex < mView.mFilter.mTargets[0].archetypes.size(), "Iterator out of bounds");
    }

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

    if (mView.mFilter.mLinkCount > 0)
    {
        // TODO: handle pins here
        ++mCursorRows[mView.mFilter.mTargetCount];
    }
    else
    {
        if (mView.mPins[0].isNull())
        {
            ++mCursorRows[0];
        }
        else
        {
            // Move iterator to the end.
            mIndex = this->endIndex();
            mCursorRows[0] = 0;
        }
    }

    this->findArchetype();
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

void QueryFilter::View::Iterator::findArchetype()
{
    auto& filter = mView.mFilter;
    auto& world = filter.mWorld;

    if (filter.mLinkCount > 0)
    {
        auto& tables = filter.mLinks[0].tables;
        while (mIndex < tables.size() &&
               mCursorRows[filter.mTargetCount] >= world.tables().sparseRelation().at(tables[mIndex]).size())
        {
            ++mIndex;

            for (int i = 0; i < filter.mTargetCount + filter.mLinkCount; ++i)
            {
                mCursorRows[i] = 0;
            }
        }

        if (mIndex < tables.size())
        {
            mTargetArchetypes[filter.mLinks[0].fromTarget] = tables[mIndex].from;
            mTargetArchetypes[filter.mLinks[0].toTarget] = tables[mIndex].to;

            // Get the entity indices of the current row.
            uint32_t fromIndex = UINT32_MAX, toIndex = UINT32_MAX;
            world.tables()
                .sparseRelation()
                .at(tables[mIndex])
                .indices(mCursorRows[filter.mTargetCount], fromIndex, toIndex);

            mCursorRows[filter.mLinks[0].fromTarget] = world.tables().dense().at(tables[mIndex].from).row(fromIndex);
            mCursorRows[filter.mLinks[0].toTarget] = world.tables().dense().at(tables[mIndex].to).row(toIndex);
        }
    }
    else
    {
        auto& archetypes = mView.mFilter.mTargets[0].archetypes;
        while (mIndex < archetypes.size() && (!world.tables().dense().contains(archetypes[mIndex]) ||
                                              mCursorRows[0] >= world.tables().dense().at(archetypes[mIndex]).size()))
        {
            ++mIndex;
            mCursorRows[0] = 0;
        }

        if (mIndex < archetypes.size())
        {
            mTargetArchetypes[0] = archetypes[mIndex];
        }
    }
}

std::size_t QueryFilter::View::Iterator::endIndex() const
{
    if (mView.mFilter.mLinkCount > 0)
    {
        return mView.mFilter.mLinks[0].tables.size();
    }

    return mView.mFilter.mTargets[0].archetypes.size();
}
