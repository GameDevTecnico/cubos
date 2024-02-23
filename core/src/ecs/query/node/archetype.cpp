#include <cubos/core/ecs/query/node/archetype.hpp>
#include <cubos/core/ecs/world.hpp>

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::QueryArchetypeNode;

QueryArchetypeNode::QueryArchetypeNode(int target)
    : QueryNode(target)
{
    this->pins(this->cursor());
}

void QueryArchetypeNode::with(ColumnId column)
{
    CUBOS_ASSERT(mBaseArchetype == ArchetypeId::Invalid, "Method can only be called before the first call to update");
    mWith.emplace_back(column);
}

void QueryArchetypeNode::without(ColumnId column)
{
    CUBOS_ASSERT(mBaseArchetype == ArchetypeId::Invalid, "Method can only be called before the first call to update");
    mWithout.emplace_back(column);
}

bool QueryArchetypeNode::equivalent(const QueryArchetypeNode& other) const
{
    if (mWith.size() != other.mWith.size() || mWithout.size() != other.mWithout.size())
    {
        return false;
    }

    for (const auto& col1 : mWith)
    {
        bool found = false;

        for (const auto& col2 : other.mWith)
        {
            if (col1 == col2)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }

    for (const auto& col1 : mWithout)
    {
        bool found = false;

        for (const auto& col2 : other.mWithout)
        {
            if (col1 == col2)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }

    return true;
}

const std::vector<ArchetypeId>& QueryArchetypeNode::archetypes() const
{
    return mArchetypes;
}

int QueryArchetypeNode::target() const
{
    return this->cursor();
}

std::size_t QueryArchetypeNode::estimate() const
{
    // TODO: use a better estimate here.
    return 1;
}

void QueryArchetypeNode::update(World& world)
{
    if (mBaseArchetype == ArchetypeId::Invalid)
    {
        mBaseArchetype = ArchetypeId::Empty;
        for (auto column : mWith)
        {
            mBaseArchetype = world.archetypeGraph().with(mBaseArchetype, column);
        }
    }

    // Collect all archetypes which have at least the required components.
    std::vector<ArchetypeId> newArchetypes;
    mSeenCount = world.archetypeGraph().collect(mBaseArchetype, newArchetypes, mSeenCount);
    mArchetypes.reserve(mArchetypes.size() + newArchetypes.size());

    // Add only the archetypes which do not have any of the excluded components.
    for (auto archetype : newArchetypes)
    {
        bool matches = true;

        for (const auto& columnId : mWithout)
        {
            if (world.archetypeGraph().contains(archetype, columnId))
            {
                matches = false;
                break;
            }
        }

        if (matches)
        {
            mArchetypes.emplace_back(archetype);
        }
    }
}

bool QueryArchetypeNode::next(World& world, TargetMask pins, Iterator& iterator) const
{
    auto cursor = this->cursor();

    if (pins & (1 << cursor))
    {
        // If the target is already pinned, we're simply validating the current match.
        // We just need to access the current archetype and check if it matches the requirements.
        auto archetype = iterator.targetArchetypes[cursor];

        for (auto column : mWith)
        {
            if (!world.archetypeGraph().contains(archetype, column))
            {
                return false;
            }
        }

        for (auto column : mWithout)
        {
            if (world.archetypeGraph().contains(archetype, column))
            {
                return false;
            }
        }

        return true;
    }

    // If we get here, then the target is not pinned and we need to iterate.
    auto& archetypeIndex = iterator.cursorIndex[cursor];
    auto& row = iterator.cursorRows[cursor];

    if (archetypeIndex >= mArchetypes.size())
    {
        // If the cursor index is out of bounds, we're starting from the beginning.
        archetypeIndex = 0;
        row = 0;
    }
    else
    {
        // Otherwise, we're just advancing the row.
        ++row;
    }

    // Advance to the next archetype as long as the current one is empty or we're out of rows.
    while (archetypeIndex < mArchetypes.size() &&
           (!world.tables().dense().contains(mArchetypes[archetypeIndex]) ||
            row >= world.tables().dense().at(mArchetypes[archetypeIndex]).size()))
    {
        ++archetypeIndex;
        row = 0;
    }

    // If there are no more archetypes, we're done.
    if (archetypeIndex == mArchetypes.size())
    {
        return false;
    }

    // Update the target's archetype.
    iterator.targetArchetypes[cursor] = mArchetypes[archetypeIndex];
    return true;
}
