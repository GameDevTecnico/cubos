#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/schedule.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::ecs::Schedule;
using cubos::core::memory::Opt;

void Schedule::clear()
{
    mNodes.clear();
}

auto Schedule::repeat(ConditionId conditionId, const Opt<NodeId>& repeatId) -> Opt<NodeId>
{
    if (auto id = this->node(repeatId))
    {
        mNodes[id->inner].isRepeat = true;
        mNodes[id->inner].conditionId.replace(conditionId);
        return id;
    }

    return {};
}

auto Schedule::system(SystemId systemId, const Opt<NodeId>& repeatId) -> Opt<NodeId>
{
    if (auto id = this->node(repeatId))
    {
        mNodes[id->inner].systemId.replace(systemId);
        return id;
    }

    return {};
}

auto Schedule::condition(ConditionId conditionId, const Opt<NodeId>& repeatId) -> Opt<NodeId>
{
    if (auto id = this->node(repeatId))
    {
        mNodes[id->inner].isRepeat = false;
        mNodes[id->inner].conditionId.replace(conditionId);
        return id;
    }

    return {};
}

auto Schedule::node(Opt<NodeId> repeatId) -> Opt<NodeId>
{
    NodeId id{mNodes.size()};

    if (repeatId.contains())
    {
        if (!mNodes[repeatId->inner].isRepeat)
        {
            return {};
        }

        mNodes[repeatId->inner].parts.push_back(id);
    }

    mNodes.push_back(Node{
        .neededSatisfaction = repeatId ? 1 : 0,
        .repeatId = repeatId,
        .depth = repeatId ? mNodes[repeatId->inner].depth + 1 : 0,
    });

    return id;
}

bool Schedule::onlyIf(NodeId node, NodeId condition)
{
    // Make sure the condition node is actually a condition node.
    if (!mNodes[condition.inner].conditionId.contains() || mNodes[condition.inner].isRepeat)
    {
        CUBOS_ERROR("Cannot make node {} run only if {} returns true, as {} isn't a condition node", node.inner,
                    condition.inner, condition.inner);
        return false;
    }

    // Make sure the condition node is at the same or lower depths as the node.
    NodeId left = node;
    NodeId right = condition;
    this->matchNodeDepths(left, right);
    if (right != condition)
    {
        CUBOS_ERROR("Cannot make node {} run only if {} returns true, as {} isn't on the same repeat chain as {}, or "
                    "is at a higher depth",
                    node.inner, condition.inner, condition.inner, node.inner);
        return false;
    }

    // Avoid cycles.
    if (this->ordered(node, condition))
    {
        CUBOS_ERROR("Cannot make node {} run only if {} returns true, as it would form a cycle, due to {} already "
                    "having to run before {}",
                    node.inner, condition.inner, node.inner, condition.inner);
        return false;
    }

    mNodes[condition.inner].satisfyOnTrue.push_back(node);
    mNodes[node.inner].neededSatisfaction += 1;
    return true;
}

bool Schedule::order(NodeId before, NodeId after)
{
    this->matchNodeDepths(before, after);

    if (before == after)
    {
        CUBOS_ERROR("Cannot make node {} run before itself", before.inner);
        return false;
    }

    if (this->ordered(before, after))
    {
        // Restriction is redundant, thus we skip adding it.
        return true;
    }

    if (this->ordered(after, before)) // NOLINT(readability-suspicious-call-argument)
    {
        // Would form a cycle.
        CUBOS_ERROR("Making node {} run before {} would form a cycle, as {} must already run before {}", before.inner,
                    after.inner, after.inner, before.inner);
        return false;
    }

    mNodes[before.inner].satisfyOnFinish.push_back(after);
    mNodes[after.inner].neededSatisfaction += 1;
    return true;
}

void Schedule::run(SystemRegistry& registry, SystemContext& context)
{
    CUBOS_DEBUG_ASSERT(mSatisfied.empty()); // Should have been cleared up by previous calls to this method.

    // Reset satisfaction level of all nodes and add them to the queue if their neededSatisfaction = 0.
    // We do this in the reverse order in order to first run systems which are added later. This makes more it more
    // noticeable when the user forgets to a specify an ordering restriction.
    for (std::size_t i = mNodes.size(); i-- > 0;)
    {
        mNodes[i].satisfaction = 0;
        mNodes[i].alreadyRepeated = false;
        mNodes[i].alreadyFinished = false;
        mNodes[i].alreadyEvaluatedToTrue = false;

        if (mNodes[i].neededSatisfaction == 0)
        {
            mSatisfied.push({i});
        }
    }

    // As long as there are satisfied nodes, execute them.
    while (!mSatisfied.empty())
    {
        this->run(registry, context, mSatisfied.front());
        mSatisfied.pop();
    }
}

void Schedule::run(SystemRegistry& registry, SystemContext& context, NodeId nodeId)
{
    auto& node = mNodes[nodeId.inner];

    if (node.systemId.contains())
    {
        // If the node is a system node, then we just run it and increment the satisfaction of dependant nodes.
        registry.system(node.systemId.value()).run(context);
        context.cmdBuffer.commit();
        this->incrementSatisfaction(node.repeatId);
        this->incrementSatisfaction(node.satisfyOnFinish);
        node.alreadyFinished = true;
        return;
    }

    // Then the node must either be a condition or repeat node. In either case, we must evaluate its associated
    // condition.
    auto result = registry.condition(node.conditionId.value()).run(context);
    context.cmdBuffer.commit();

    if (!node.isRepeat)
    {
        // If the node is a condition node, then independently of its result, we should increment the satisfaction for
        // nodes which depend on it being evaluated.
        this->incrementSatisfaction(node.repeatId);
        this->incrementSatisfaction(node.satisfyOnFinish);
        node.alreadyFinished = true;

        if (result)
        {
            // If it evaluated to true, then also increment the satisfaction of nodes which depend on it.
            this->incrementSatisfaction(node.satisfyOnTrue);
            node.alreadyEvaluatedToTrue = true;
            return;
        }

        // Otherwise, increment the satisfaction of nodes which depend on the nodes which will be skipped.
        for (auto skippedId : node.satisfyOnTrue)
        {
            this->skip(node.repeatId, skippedId);
        }

        return;
    }

    if (node.alreadyRepeated)
    {
        // If the repeat node has previously been run, then we need to reset it parts state.
        for (const auto& part : node.parts)
        {
            mNodes[part.inner].satisfaction -= 1;

            mNodes[part.inner].alreadyRepeated = false;

            if (mNodes[part.inner].alreadyFinished)
            {
                this->decrementSatisfaction(mNodes[part.inner].satisfyOnFinish);
                mNodes[part.inner].alreadyFinished = false;
            }

            if (mNodes[part.inner].alreadyEvaluatedToTrue)
            {
                this->decrementSatisfaction(mNodes[part.inner].satisfyOnTrue);
                mNodes[part.inner].alreadyEvaluatedToTrue = false;
            }
        }
    }

    node.isRepeating = result;

    if (!result)
    {
        node.skippedParts = 0;

        // If the condition evaluated to false, then increase the satisfaction of nodes waiting for it to finish.
        this->incrementSatisfaction(node.repeatId);
        this->incrementSatisfaction(node.satisfyOnFinish);
        node.alreadyFinished = true;
        return;
    }

    // Then the condition evaluated to true.
    node.alreadyRepeated = true;

    // Make the part nodes run.
    this->incrementSatisfaction(node.parts);

    // Decrease the satisfaction of this node so that it runs after all of its parts finish.
    // CUBOS_DEBUG_ASSERT(node.satisfaction == node.neededSatisfaction);
    node.satisfaction -= static_cast<int>(node.parts.size()); // Satisfaction may become negative here, but thats okay!
    node.satisfaction += node.skippedParts;
    if (node.satisfaction == node.neededSatisfaction)
    {
        // If repeat node is empty, it can run again immediately.
        mSatisfied.push(nodeId);
    }
}

std::string Schedule::debug(const SystemRegistry& registry) const
{
    std::vector<std::string> lines{};
    std::vector<bool> visited(mNodes.size(), false);

    auto visit = [&](NodeId id, int indent, auto& self) {
        if (visited[id.inner])
        {
            return;
        }

        visited[id.inner] = true;

        // First visit all nodes which can only run after this node finishes.
        for (auto onFinish : mNodes[id.inner].satisfyOnFinish)
        {
            self(onFinish, indent, self);
        }

        for (auto onTrue : mNodes[id.inner].satisfyOnTrue)
        {
            self(onTrue, indent, self);
        }

        // Add indentation.
        std::string line{};
        for (int i = 0; i < indent; ++i)
        {
            line += "  ";
        }
        line += "- ";

        if (mNodes[id.inner].systemId)
        {
            // If the node is a system, simply print the system name.
            line += registry.name(mNodes[id.inner].systemId.value());
        }
        else if (!mNodes[id.inner].isRepeat)
        {
            // If the node is a condition, print the condition name and the result.
            line += registry.name(mNodes[id.inner].conditionId.value());
        }
        else
        {
            // If the node is a repeat node, visit its parts, and print the condition name.
            for (auto part : mNodes[id.inner].parts)
            {
                self(part, indent + 1, self);
            }

            line += registry.name(mNodes[id.inner].conditionId.value());
        }

        lines.push_back(line);
    };

    for (std::size_t i = 0; i < mNodes.size(); ++i)
    {
        visit({i}, 0, visit);
    }

    // Reverse the lines so that the output is in the correct order.
    std::string result{};
    for (int i = static_cast<int>(lines.size()) - 1; i >= 0; --i)
    {
        if (!result.empty())
        {
            result += '\n';
        }

        result += lines[static_cast<std::size_t>(i)];
    }
    return result;
}

bool Schedule::ordered(NodeId before, NodeId right) const
{
    this->matchNodeDepths(before, right);

    for (const auto& node : mNodes[before.inner].satisfyOnFinish)
    {
        if (node == right || this->ordered(node, right))
        {
            return true;
        }
    }

    for (const auto& node : mNodes[before.inner].satisfyOnTrue)
    {
        if (node == right || this->ordered(node, right))
        {
            return true;
        }
    }

    return false;
}

auto Schedule::rootRepeatNode(NodeId node) const -> NodeId
{
    while (mNodes[node.inner].repeatId.contains())
    {
    }

    return node;
}

void Schedule::matchNodeDepths(NodeId& left, NodeId& right) const
{
    while (mNodes[left.inner].repeatId != mNodes[right.inner].repeatId)
    {
        if (mNodes[left.inner].depth > mNodes[right.inner].depth)
        {
            left = mNodes[left.inner].repeatId.value();
        }
        else
        {
            right = mNodes[right.inner].repeatId.value();
        }
    }
}

void Schedule::skip(const Opt<NodeId>& repeatId, NodeId nodeId)
{
    auto& node = mNodes[nodeId.inner];

    if (repeatId != node.repeatId)
    {
        mNodes[node.repeatId->inner].skippedParts += 1;
    }

    if (node.repeatId && mNodes[node.repeatId->inner].isRepeating)
    {
        this->incrementSatisfaction(node.repeatId);
    }

    if (node.conditionId.contains())
    {
        for (auto skippedId : node.satisfyOnTrue)
        {
            this->skip(repeatId, skippedId);
        }
    }

    this->incrementSatisfaction(mNodes[nodeId.inner].satisfyOnFinish);
}

void Schedule::incrementSatisfaction(Opt<NodeId> node)
{
    if (node.contains())
    {
        mNodes[node->inner].satisfaction += 1;

        if (mNodes[node->inner].satisfaction == mNodes[node->inner].neededSatisfaction)
        {
            mSatisfied.push(node.value());
        }
    }
}

void Schedule::incrementSatisfaction(const std::vector<NodeId>& nodes)
{
    // We increment the satisfaction in the reverse order, so that we first run nodes whose restrictions were added
    // later. This makes it easier to spot incomplete configurations.
    for (std::size_t i = nodes.size(); i-- > 0;)
    {
        this->incrementSatisfaction(nodes[i]);
    }
}

void Schedule::decrementSatisfaction(const std::vector<NodeId>& nodes)
{
    for (const auto& node : nodes)
    {
        mNodes[node.inner].satisfaction -= 1;
    }
}
