#include <cubos/core/ecs/query/filter.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::QueryFilter;

QueryFilter::QueryFilter(World& world, const std::vector<QueryTerm>& terms)
    : mWorld{world}
    , mTerms{terms}
{
    // Count how many targets we have.
    for (const auto& term : terms)
    {
        if (term.isEntity())
        {
            mTargetCount = std::max(mTargetCount, term.entity.target + 1);
            mTermCursors.emplace_back(term.entity.target);
        }
        else if (term.isComponent(world.types()))
        {
            mTargetCount = std::max(mTargetCount, term.component.target + 1);
            mTermCursors.emplace_back(term.component.target);
        }
        else
        {
            CUBOS_UNREACHABLE();
        }
    }

    // Make sure we can handle them.
    CUBOS_ASSERT(mTargetCount <= MaxTargetCount, "Currently only targets from 0 to {} are supported",
                 MaxTargetCount - 1);

    // Right now the number of cursors always matches the number of targets.
    mCursorCount = mTargetCount;

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
    mArchetypeIndex = 0;
    mCursorRows[0] = 0;

    if (end)
    {
        mArchetypeIndex = mView.mFilter.mTargets[0].archetypes.size();
    }
    else
    {
        // Set the archetype and cursor row for each of the pinned targets.
        for (int target = 0; target < mView.mFilter.mTargetCount; ++target)
        {
            auto pin = mView.mPins[target];
            if (!pin.isNull())
            {
                mArchetypeIndex = mView.mFilter.mTargets[0].archetypes.size();

                // Find archetype index.
                auto archetype = mView.mFilter.mWorld.archetype(pin);
                for (std::size_t i = 0; i < mView.mFilter.mTargets[0].archetypes.size(); ++i)
                {
                    if (mView.mFilter.mTargets[0].archetypes[i] == archetype)
                    {
                        mArchetypeIndex = i;
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
    for (int target = 0; target < mView.mFilter.mTargetCount; ++target)
    {
        if (mTargetArchetypes[target] != other.mTargetArchetypes[target])
        {
            return false;
        }
    }

    for (int cursor = 0; cursor < mView.mFilter.mCursorCount; ++cursor)
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
    CUBOS_ASSERT(mArchetypeIndex < mView.mFilter.mTargets[0].archetypes.size(), "Iterator out of bounds");
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
    CUBOS_ASSERT(mArchetypeIndex < mView.mFilter.mTargets[0].archetypes.size(), "Iterator out of bounds");

    if (mView.mPins[0].isNull())
    {
        ++mCursorRows[0];
    }
    else
    {
        // Move iterator to the end.
        mArchetypeIndex = mView.mFilter.mTargets[0].archetypes.size();
        mCursorRows[0] = 0;
    }

    this->findArchetype();
    return *this;
}

const ArchetypeId* QueryFilter::View::Iterator::targetArchetypes() const
{
    CUBOS_ASSERT(mArchetypeIndex < mView.mFilter.mTargets[0].archetypes.size(), "Iterator out of bounds");
    return mTargetArchetypes;
}

const std::size_t* QueryFilter::View::Iterator::cursorRows() const
{
    CUBOS_ASSERT(mArchetypeIndex < mView.mFilter.mTargets[0].archetypes.size(), "Iterator out of bounds");
    return mCursorRows;
}

void QueryFilter::View::Iterator::findArchetype()
{
    auto& world = mView.mFilter.mWorld;
    auto& archetypes = mView.mFilter.mTargets[0].archetypes;
    while (mArchetypeIndex < archetypes.size() &&
           (!world.tables().dense().contains(archetypes[mArchetypeIndex]) ||
            mCursorRows[0] >= world.tables().dense().at(archetypes[mArchetypeIndex]).size()))
    {
        ++mArchetypeIndex;
        mCursorRows[0] = 0;
    }

    if (mArchetypeIndex < archetypes.size())
    {
        mTargetArchetypes[0] = archetypes[mArchetypeIndex];
    }
    else
    {
        mTargetArchetypes[0] = ArchetypeId::Invalid;
    }
}
