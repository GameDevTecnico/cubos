#include <algorithm>

#include <cubos/core/ecs/system/planner.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::ecs::Planner;

auto Planner::add() -> TagId
{
    mTags.emplace_back();
    return {mTags.size() - 1};
}

auto Planner::add(SystemId systemId) -> TagId
{
    mTags.push_back(TagData{.systemId = systemId});
    return {mTags.size() - 1};
}

void Planner::remove(TagId tagId)
{
    if (mTags[tagId.inner].isNegative)
    {
        CUBOS_ERROR("Negative tags cannot be removed");
        return;
    }

    // Remove references to the tag or its negative counterpart from other tags.
    for (auto& tag : mTags)
    {
        if (tag.repeatParentId.contains() &&
            (tag.repeatParentId.value() == tagId || tag.repeatParentId == mTags[tagId.inner].negatedId))
        {
            tag.repeatParentId.reset();
        }

        std::erase_if(tag.parents, [&](const TagId& id) { return id == tagId; });
        std::erase_if(tag.after, [&](const TagId& id) { return id == tagId; });

        if (tag.negatedId.contains())
        {
            std::erase_if(tag.parents, [&](const TagId& id) { return id == tag.negatedId.value(); });
            std::erase_if(tag.after, [&](const TagId& id) { return id == tag.negatedId.value(); });
        }
    }

    // Remove any data from the tag itself.
    mTags[tagId.inner].systemId.reset();
    mTags[tagId.inner].conditionIds.clear();
    mTags[tagId.inner].repeatConditionId.reset();
    mTags[tagId.inner].repeatParentId.reset();
    mTags[tagId.inner].parents.clear();
    mTags[tagId.inner].after.clear();

    // And from its negative counterpart.
    if (auto negatedId = mTags[tagId.inner].negatedId)
    {
        mTags[negatedId->inner].systemId.reset();
        mTags[negatedId->inner].conditionIds.clear();
        mTags[negatedId->inner].repeatConditionId.reset();
        mTags[negatedId->inner].repeatParentId.reset();
        mTags[negatedId->inner].parents.clear();
        mTags[negatedId->inner].after.clear();
    }
}

auto Planner::negate(TagId tagId) -> TagId
{
    if (!mTags[tagId.inner].negatedId.contains())
    {
        CUBOS_ASSERT(!mTags[tagId.inner].isNegative);

        mTags.push_back(TagData{
            .isNegative = true,
            .negatedId = tagId,
        });

        mTags[tagId.inner].negatedId.replace({mTags.size() - 1});
    }

    return mTags[tagId.inner].negatedId.value();
}

bool Planner::tag(TagId childId, TagId parentId)
{
    if (childId == parentId)
    {
        return true;
    }

    // If parentId is a repeat tag:
    // - if childId isn't in a repeat tag yet, or its repeat tag is a parent of parent, set it's repeat tag to parent
    // - otherwise, if its repeat tag is a child of parent, don't do anything
    // - otherwise, log an error and return false, as the tag would need to be added to two repeat groups

    // When the repeat tag changes, we must check that

    // Make

    // TODO: parentId is already a

    // Check in which repeat id we're putting the tag in, if any.
    // If the parent tag itself is a repeating tag, then we choose it. Otherwise, we check if the parent is part of a
    // repeating tag, and use that one , if there's one.
    auto parentRepeatId = mTags[parentId.inner].repeatConditionId ? parentId : mTags[parentId.inner].repeatParentId;
    if (parentRepeatId)
    {
        if (mTags[childId.inner].repeatParentId)
        {
            // The child tag is already part of a repeat group. Thus this tag is only valid if one of the tags is an
            // ancestor of the other. The child becomes part of the deepest repeat of the two.
            // TODO
        }
        else
        {
            mTags[childId.inner].repeatParentId = parentRepeatId;
            mTags[childId.inner].depth += 1;

            // TODO: update depth of all tags which have this tag as a parent
        }
    }

    mTags[childId.inner].parents.push_back(parentId);
}

bool Planner::order(TagId beforeId, TagId afterId)
{
    (void)beforeId;
    (void)afterId;
    CUBOS_TODO();
}

void Planner::onlyIf(TagId tagId, ConditionId conditionId)
{
    (void)tagId;
    (void)conditionId;
    CUBOS_TODO();
}

bool Planner::repeatWhile(TagId tagId, ConditionId conditionId)
{
    (void)tagId;
    (void)conditionId;
    CUBOS_TODO();
}

auto Planner::build() const -> Schedule
{
    CUBOS_TODO();
}

bool Planner::repeatsWithin(TagId tagId, TagId repeatId) const
{
    auto& tag = mTags[tagId.inner];

    if (!tag.repeatParentId.contains())
    {
        return false;
    }

    return tag.repeatParentId.value() == repeatId || this->repeatsWithin(tag.repeatParentId.value(), repeatId);
}

bool Planner::isValidAncestor(TagId tagId, TagId ancestorId) const
{
    const auto& tag = mTags[tagId.inner];

    //

    // Make sure the children of the tag would still be valid if tagged with ancestorId.
    for (auto childId : tag.children)
    {
        if (childId == ancestorId)
        {
            // Would form a cycle!
            return false;
        }

        if (!this->isValidAncestor(childId, ancestorId))
        {
            return false;
        }
    }

    return true;
}