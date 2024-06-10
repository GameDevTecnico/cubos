#include <algorithm>

#include <cubos/core/ecs/system/planner.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::ecs::Planner;
using cubos::core::memory::Opt;

std::size_t Planner::TagId::Hash::operator()(const TagId& tagId) const
{
    return tagId.inner;
}

void Planner::clear()
{
    mTags.clear();
}

auto Planner::add() -> TagId
{
    return this->add("unnamed" + std::to_string(mTags.size()));
}

auto Planner::add(std::string name) -> TagId
{
    mTags.push_back(TagData{.name = name});
    return {mTags.size() - 1};
}

auto Planner::add(std::string name, SystemId systemId) -> TagId
{
    mTags.push_back(TagData{.name = name, .systemId = systemId});
    return {mTags.size() - 1};
}

void Planner::remove(TagId tagId)
{
    // Remove references to the tag or its negative counterpart from other tags.
    for (auto& tag : mTags)
    {
        std::erase_if(tag.parents, [&](const TagId& id) { return id == tagId; });
        std::erase_if(tag.children, [&](const TagId& id) { return id == tagId; });
        std::erase_if(tag.after, [&](const TagId& id) { return id == tagId; });
    }

    // Remove any data from the tag itself.
    mTags[tagId.inner].systemId.reset();
    mTags[tagId.inner].conditionIds.clear();
    mTags[tagId.inner].repeatConditionId.reset();
    mTags[tagId.inner].parents.clear();
    mTags[tagId.inner].children.clear();
    mTags[tagId.inner].after.clear();
}

bool Planner::tag(TagId childId, TagId parentId)
{
    if (mTags[parentId.inner].systemId.contains())
    {
        CUBOS_ERROR("Leaf tags (systems) cannot be used as parent tags");
        return false;
    }

    mTags[childId.inner].parents.insert(parentId);
    mTags[parentId.inner].children.insert(childId);
    return true;
}

void Planner::order(TagId beforeId, TagId afterId)
{
    mTags[afterId.inner].after.insert(beforeId);
}

void Planner::onlyIf(TagId tagId, ConditionId conditionId)
{
    mTags[tagId.inner].conditionIds.push_back(conditionId);
}

bool Planner::repeatWhile(TagId tagId, ConditionId conditionId)
{
    if (mTags[tagId.inner].repeatConditionId.contains())
    {
        CUBOS_ERROR("Could not make {} a repeating tag, as it was already a repeating tag", tagId.inner);
        return false;
    }

    mTags[tagId.inner].repeatConditionId.replace(conditionId);
    return true;
}

auto Planner::build() const -> Opt<Schedule>
{
    // Create a copy of the tag data, and collapse all of the tags.
    // Results in each tag having all of the properties of its parents, and its repeating parent field set.
    auto tags = mTags;
    std::vector<bool> visited(tags.size(), false);
    for (std::size_t i = 0; i < tags.size(); ++i)
    {
        if (!Planner::collapse(TagId{.inner = i}, tags, visited))
        {
            return {};
        }
    }

    // Expand the children fields of all tags to include the children of their children.
    // Also change the after field to contain the children of all tags in it.
    // After this step, the parent and children tags are no longer relevant.
    visited.clear();
    visited.resize(tags.size(), false);
    for (std::size_t i = 0; i < tags.size(); ++i)
    {
        Planner::expand(TagId{.inner = i}, tags, visited);
    }

    // Make indirect ordering constraints which pass through non-leaf and non-repeat tags direct constraints.
    // This solves the problem of having a system A run before a tag T, which in turn runs before a system B.
    // If we didn't do this, after removing the tag T, the indirect ordering between A and B would be lost.
    visited.clear();
    visited.resize(tags.size(), false);
    for (std::size_t i = 0; i < tags.size(); ++i)
    {
        Planner::addMissingOrdering(TagId{.inner = i}, tags, visited);
    }

    // Create nodes for each all leaf and repeating tags.
    Schedule schedule{};
    std::vector<Opt<Schedule::NodeId>> systemAndRepeatNodes{tags.size()};
    std::vector<std::vector<Schedule::NodeId>> allNodes{tags.size()};
    for (std::size_t i = 0; i < tags.size(); ++i)
    {
        Planner::makeSystemOrRepeatNode(TagId{.inner = i}, schedule, tags, systemAndRepeatNodes);

        if (systemAndRepeatNodes[i])
        {
            allNodes[i].push_back(systemAndRepeatNodes[i].value());
        }
    }

    // Create nodes for all conditions.
    std::vector<std::vector<Schedule::NodeId>> conditionNodes{tags.size()};
    for (std::size_t i = 0; i < tags.size(); ++i)
    {
        for (auto conditionId : tags[i].conditionIds)
        {
            // Get the repeating node the condition we'll be part of, if any.
            Opt<Schedule::NodeId> repeatNodeId{};
            if (tags[i].repeatingParent)
            {
                repeatNodeId.replace(systemAndRepeatNodes[tags[i].repeatingParent->inner].value());
            }

            auto conditionNodeId = schedule.condition(conditionId, repeatNodeId).value();
            conditionNodes[i].push_back(conditionNodeId);
            allNodes[i].push_back(conditionNodeId);
        }
    }

    // Iterate through all tags and add their ordering and condition constraints to their nodes.
    for (std::size_t i = 0; i < tags.size(); ++i)
    {
        // Add tag ordering constraints it to all of its nodes.
        for (auto beforeTagId : tags[i].after)
        {
            for (auto beforeNodeId : allNodes[beforeTagId.inner])
            {
                for (auto afterNodeId : allNodes[i])
                {
                    if (!schedule.order(beforeNodeId, afterNodeId))
                    {
                        CUBOS_ERROR(
                            "Couldn't enforce ordering constraint between {} and {}, as it would create a cycle",
                            tags[beforeTagId.inner].name, tags[i].name);
                        return {};
                    }
                }
            }
        }

        // If any of the parents have associated conditions, make the nodes depend on them.
        for (const auto& parentId : tags[i].parents)
        {
            for (auto conditionNodeId : conditionNodes[parentId.inner])
            {
                for (auto nodeId : allNodes[i])
                {
                    CUBOS_ASSERT(schedule.onlyIf(nodeId, conditionNodeId));
                }
            }
        }

        // If the tag has any associated conditions, make its nodes depend on them.
        for (auto conditionNodeId : conditionNodes[i])
        {
            for (auto nodeId : allNodes[i])
            {
                // Make sure this node isn't a condition node itself.
                bool shouldAdd = true;
                for (auto otherConditionNodeId : conditionNodes[i])
                {
                    if (nodeId == otherConditionNodeId)
                    {
                        shouldAdd = false;
                        break;
                    }
                }

                if (shouldAdd)
                {
                    CUBOS_ASSERT(schedule.onlyIf(nodeId, conditionNodeId));
                }
            }
        }
    }

    return schedule;
}

bool Planner::repeatsWithin(TagId tagId, TagId repeatingId, const std::vector<TagData>& tags)
{
    const auto& tag = tags[tagId.inner];
    return tag.repeatingParent && (tag.repeatingParent.value() == repeatingId ||
                                   Planner::repeatsWithin(tag.repeatingParent.value(), repeatingId, tags));
}

bool Planner::collapse(TagId tagId, std::vector<TagData>& tags, std::vector<bool>& visited)
{
    // Only visit each tag once.
    if (visited[tagId.inner])
    {
        return true;
    }

    visited[tagId.inner] = true;
    auto& tag = tags[tagId.inner];

    // Collapse parent tags.
    for (auto parentId : tag.parents)
    {
        if (!Planner::collapse(parentId, tags, visited))
        {
            return false;
        }
    }

    // Copy properties from parent tags to self. We copy the parent set as it will modified during iteration.
    auto parents = tag.parents;
    for (const auto& parentId : parents)
    {
        if (parentId == tagId)
        {
            CUBOS_ERROR("{} is tagged with itself, which is forbidden", tag.name);
            return false;
        }

        const auto& parent = tags[parentId.inner];

        auto repeatingParent = parent.repeatConditionId ? parentId : parent.repeatingParent;
        if (repeatingParent)
        {
            if (!tag.repeatingParent ||
                Planner::repeatsWithin(repeatingParent.value(), tag.repeatingParent.value(), tags))
            {
                // If the tag didn't already have a repeating parent, or if the new repeating parent is part of the
                // previous, then set it as the current repeating parent.
                tag.repeatingParent.replace(repeatingParent.value());
            }
            else if (!Planner::repeatsWithin(tag.repeatingParent.value(), repeatingParent.value(), tags))
            {
                // If the tag already had a repeating parent, and the new repeating parent isn't a parent of the
                // previous, then the user made the tag belong to two distinct repeating tags.
                CUBOS_ERROR("{} belongs to two repeating tags {} and {}, which is forbidden", tag.name,
                            tags[tag.repeatingParent->inner].name, tags[repeatingParent->inner].name);
                return false;
            }
        }

        for (auto grandParentId : parent.parents)
        {
            if (parentId == grandParentId)
            {
                CUBOS_ERROR("{} is tagged with itself through {}, which is forbidden", tag.name, parent.name);
                return false;
            }

            tag.parents.insert(grandParentId);
        }

        for (auto after : parent.after)
        {
            tag.after.insert(after);
        }
    }

    return true;
}

void Planner::expand(TagId tagId, std::vector<TagData>& tags, std::vector<bool>& visited)
{
    // Only visit each tag once.
    if (visited[tagId.inner])
    {
        return;
    }

    visited[tagId.inner] = true;
    auto& tag = tags[tagId.inner];

    // Recurse into all children, and add their children to our own child set.
    auto children = tag.children;
    for (auto childId : children)
    {
        Planner::expand(childId, tags, visited);

        for (auto grandChildId : tags[childId.inner].children)
        {
            tag.children.insert(grandChildId);
        }
    }

    // Do the same but for the after constraint.
    auto after = tag.after;
    for (auto afterId : after)
    {
        Planner::expand(afterId, tags, visited);

        for (auto afterChildId : tags[afterId.inner].children)
        {
            tag.after.insert(afterChildId);
        }
    }
}

void Planner::addMissingOrdering(TagId tagId, std::vector<TagData>& tags, std::vector<bool>& visited)
{
    // Only visit each tag once.
    if (visited[tagId.inner])
    {
        return;
    }

    visited[tagId.inner] = true;
    auto& tag = tags[tagId.inner];

    // For each tag which must run before this tag (we copy the set as it will be modified while iterating).
    auto after = tag.after;
    for (auto beforeId : after)
    {
        // Recurse into it first.
        Planner::addMissingOrdering(beforeId, tags, visited);

        // Skip the tag if it is a leaf or repeat tag (i.e., skip tags which have nodes).
        const auto& before = tags[beforeId.inner];
        if (before.repeatConditionId.contains() || before.systemId.contains())
        {
            continue;
        }

        // Since the tag won't have a node in the final schedule, we must add its ordering constraints to this tag.
        for (auto transitiveBeforeId : before.after)
        {
            tag.after.insert(transitiveBeforeId);
        }
    }
}

void Planner::makeSystemOrRepeatNode(TagId tagId, Schedule& schedule, const std::vector<TagData>& tags,
                                     std::vector<Opt<Schedule::NodeId>>& nodes)
{
    if (nodes[tagId.inner])
    {
        return;
    }

    auto& tag = tags[tagId.inner];

    // Get the repeating node the new node will belong to.
    Opt<Schedule::NodeId> repeatingNodeId{};
    if (tag.repeatingParent)
    {
        auto repeatingTagId = tag.repeatingParent.value();
        Planner::makeSystemOrRepeatNode(repeatingTagId, schedule, tags, nodes);
        repeatingNodeId.replace(nodes[repeatingTagId.inner].value());
    }

    if (tag.repeatConditionId)
    {
        // If tag is a repeating tag, create a repeating node.
        nodes[tagId.inner].replace(schedule.repeat(tag.repeatConditionId.value(), repeatingNodeId).value());
    }
    else if (tag.systemId)
    {
        // Otherwise, if the tag is a leaf tag, create a system node.
        nodes[tagId.inner].replace(schedule.system(tag.systemId.value(), repeatingNodeId).value());
    }
}
