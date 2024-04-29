#include <algorithm>

#include <cubos/core/ecs/query/filter.hpp>
#include <cubos/core/ecs/query/node/archetype.hpp>
#include <cubos/core/ecs/query/node/related.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::QueryFilter;

QueryFilter::~QueryFilter()
{
    for (int i = 0; i < mNodeCount; ++i)
    {
        delete mNodes[i];
    }
}

QueryFilter::QueryFilter(World& world, const std::vector<QueryTerm>& terms)
    : mWorld{world}
{
    CUBOS_DEBUG("Creating query filter with terms: {}", QueryTerm::toString(world.types(), terms));

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
    CUBOS_ASSERT(mTargetCount <= QueryNode::MaxTargetCount, "Currently only targets from 0 to {} are supported",
                 QueryNode::MaxTargetCount - 1);

    // For each target, create a new archetype node.
    for (int target = 0; target < mTargetCount; ++target)
    {
        mNodes[mNodeCount++] = new QueryArchetypeNode(target);
    }

    // Go through each component term and initialize the respective archetype node.
    for (const auto& term : terms)
    {
        // Filter terms which aren't component terms or which are optional.
        if (!term.isComponent(world.types()) || term.component.optional)
        {
            continue;
        }

        // Add the respective column restriction.
        auto& node = *static_cast<QueryArchetypeNode*>(mNodes[term.component.target]);
        auto columnId = ColumnId::make(term.type);
        if (term.component.without)
        {
            node.without(columnId);
        }
        else
        {
            node.with(columnId);
        }
    }

    // Go through each term and assign a cursor to each of them. Also initialize relation terms.
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
            auto cursor = mNodeCount++;
            CUBOS_ASSERT(cursor < QueryNode::MaxCursorCount, "Only at most {} cursors are supported",
                         QueryNode::MaxCursorCount);
            CUBOS_ASSERT(term.relation.fromTarget < mTargetCount, "No such target {}", term.relation.fromTarget);
            CUBOS_ASSERT(term.relation.toTarget < mTargetCount, "No such target {}", term.relation.toTarget);

            mTermCursors.emplace_back(cursor);

            auto& fromNode = *static_cast<QueryArchetypeNode*>(mNodes[term.relation.fromTarget]);
            auto& toNode = *static_cast<QueryArchetypeNode*>(mNodes[term.relation.toTarget]);
            auto includeDuplicates = world.types().isSymmetricRelation(term.type) && !fromNode.equivalent(toNode);
            mNodes[cursor] = new QueryRelatedNode(cursor, term.type, world.types().isSymmetricRelation(term.type),
                                                  includeDuplicates, term.relation.traversal, fromNode, toNode);
        }
        else
        {
            CUBOS_UNREACHABLE();
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
    // Update the cache of each node.
    for (int i = 0; i < mNodeCount; ++i)
    {
        mNodes[i]->update(mWorld);
    }

    // Sort by nodes by their estimate, such that the nodes with the least matches are processed first.
    std::sort(mNodes, mNodes + mNodeCount,
              [](QueryNode* lhs, QueryNode* rhs) { return lhs->estimate() < rhs->estimate(); });

    // Update the pin masks with which each node will run.
    mNodePins[0] = 0;
    for (int i = 1; i < mNodeCount; ++i)
    {
        mNodePins[i] = mNodePins[i - 1] | mNodes[i - 1]->pins();
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

QueryFilter::View::View(QueryFilter& filter, QueryNode::TargetMask pinMask)
    : mFilter{filter}
    , mPinMask{pinMask}
{
    for (mIncNode = -1; mIncNode < mFilter.mNodeCount - 1; ++mIncNode)
    {
        auto currentPins = mFilter.mNodePins[mIncNode + 1] | mPinMask;

        // If all of the targets of the next node are pinned, then we can break here.
        if ((mFilter.mNodes[mIncNode + 1]->pins() & currentPins) == mFilter.mNodes[mIncNode + 1]->pins())
        {
            break;
        }
    }
}

auto QueryFilter::View::operator=(const View& view) -> View&
{
    CUBOS_ASSERT(&mFilter == &view.mFilter);

    for (int target = 0; target < view.mFilter.mTargetCount; ++target)
    {
        mPinEntities[target] = view.mPinEntities[target];
    }
    mPinMask = view.mPinMask;

    return *this;
}

auto QueryFilter::View::pin(int target, Entity entity) -> View
{
    CUBOS_ASSERT(target < mFilter.mTargetCount, "No such target {} in query", target);
    CUBOS_ASSERT(mPinEntities[target].isNull(), "Target {} was already pinned", target);

    auto pinMask = mPinMask;
    pinMask |= 1 << target;
    View view{mFilter, pinMask};

    // If the entity is dead, set the view as dead, to guarantee that the iterator will not return any matches.
    view.mDead = mDead;
    if (!mFilter.mWorld.isAlive(entity))
    {
        view.mDead = true;
    }

    // Pass the pins from the current view to the new view.
    for (int i = 0; i < mFilter.mTargetCount; ++i)
    {
        view.mPinEntities[i] = mPinEntities[i];
    }
    view.mPinEntities[target] = entity;
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
    if (end)
    {
        mNodeIndex = -1;
    }
    else
    {
        mNodeIndex = 0;

        // Reset all cursors.
        for (int i = 0; i < mView.mFilter.mNodeCount; ++i)
        {
            mIterator.cursorIndex[i] = SIZE_MAX;
            mIterator.cursorRows[i] = 0;
        }

        // Initialize cursors for pinned targets.
        for (int i = 0; i < mView.mFilter.mTargetCount; ++i)
        {
            if (!mView.mPinEntities[i].isNull())
            {
                auto ent = mView.mPinEntities[i];
                auto archetype = mView.mFilter.mWorld.archetype(ent);
                mIterator.targetArchetypes[i] = archetype;
                mIterator.cursorRows[i] = mView.mFilter.mWorld.tables().dense().at(archetype).row(ent.index);
            }
        }

        // Advance the iterator until it points to the first match.
        this->next();
    }
}

bool QueryFilter::View::Iterator::operator==(const Iterator& other) const
{
    if (&mView != &other.mView || mNodeIndex != other.mNodeIndex)
    {
        return false;
    }

    for (int cursor = 0; cursor <= mNodeIndex; ++cursor)
    {
        if (mIterator.cursorIndex[cursor] != other.mIterator.cursorIndex[cursor] ||
            mIterator.cursorRows[cursor] != other.mIterator.cursorRows[cursor])
        {
            return false;
        }
    }

    return true;
}

auto QueryFilter::View::Iterator::operator*() const -> const Match&
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");

    const auto& world = mView.mFilter.mWorld;

    for (int target = 0; target < mView.mFilter.mTargetCount; ++target)
    {
        auto archetype = mIterator.targetArchetypes[target];
        auto row = mIterator.cursorRows[target];
        mMatch.entities[target].index = world.tables().dense().at(archetype).entity(row);
        mMatch.entities[target].generation = world.generation(mMatch.entities[target].index);
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

    if (mView.mIncNode == -1)
    {
        // All targets are pinned, there can't be any more matches.
        mNodeIndex = -1;
    }
    else
    {
        this->next();
    }

    return *this;
}

const ArchetypeId* QueryFilter::View::Iterator::targetArchetypes() const
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");
    return mIterator.targetArchetypes;
}

const int* QueryFilter::View::Iterator::cursorDepths() const
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");
    return mIterator.cursorDepths;
}

const std::size_t* QueryFilter::View::Iterator::cursorRows() const
{
    CUBOS_ASSERT(this->valid(), "Iterator out of bounds");
    return mIterator.cursorRows;
}

bool QueryFilter::View::Iterator::valid() const
{
    return mNodeIndex >= 0;
}

void QueryFilter::View::Iterator::next()
{
    for (;;)
    {
        if (mView.mFilter.mNodes[mNodeIndex]->next(mView.mFilter.mWorld,
                                                   mView.mFilter.mNodePins[mNodeIndex] | mView.mPinMask, mIterator))
        {
            // Otherwise, move on to the next node.
            mNodeIndex += 1;

            // If this was the last node and it succeeded, then we've found a new match.
            if (mNodeIndex == mView.mFilter.mNodeCount)
            {
                if (mView.mIncNode != -1)
                {
                    // Reset the node index to the last iterating node.
                    mNodeIndex = mView.mIncNode;
                }

                return;
            }
        }
        else
        {
            mIterator.cursorIndex[mView.mFilter.mNodes[mNodeIndex]->cursor()] = SIZE_MAX;
            mIterator.cursorRows[mView.mFilter.mNodes[mNodeIndex]->cursor()] = 0;

            // Backtrack to the previous iterating node.
            if (mNodeIndex > mView.mIncNode)
            {
                mNodeIndex = mView.mIncNode;
            }
            else
            {
                mNodeIndex -= 1;
            }

            if (mNodeIndex == -1)
            {
                return;
            }
        }
    }
}
