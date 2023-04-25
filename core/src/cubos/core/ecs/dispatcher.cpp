#include <algorithm>

#include <cubos/core/ecs/dispatcher.hpp>

using namespace cubos::core::ecs;

void Dispatcher::SystemSettings::copyFrom(const SystemSettings* other)
{
    std::unique_copy(other->before.tag.begin(), other->before.tag.end(), std::back_inserter(this->before.tag));
    std::unique_copy(other->after.tag.begin(), other->after.tag.end(), std::back_inserter(this->after.tag));
    std::unique_copy(other->before.system.begin(), other->before.system.end(), std::back_inserter(this->before.system));
    std::unique_copy(other->after.system.begin(), other->after.system.end(), std::back_inserter(this->after.system));
    this->conditions |= other->conditions;
}

Dispatcher::~Dispatcher()
{
    for (System* system : systems)
    {
        delete system;
    }
}

void Dispatcher::addTag(const std::string& tag)
{
    ENSURE_TAG_SETTINGS(tag);
    currTag = tag;
}

void Dispatcher::tagInheritTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    ENSURE_TAG_SETTINGS(tag);
    if (std::find(tagSettings[currTag]->inherits.begin(), tagSettings[currTag]->inherits.end(), tag) !=
        tagSettings[currTag]->inherits.end())
    {
        CUBOS_INFO("Tag already inherits from '{}'", tag);
    }
    tagSettings[currTag]->inherits.push_back(tag);
}

void Dispatcher::tagSetAfterTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    ENSURE_TAG_SETTINGS(tag);
    // Set curr to run after this tag
    tagSettings[currTag]->after.tag.push_back(tag);
    // And that tag to run before this tag
    tagSettings[tag]->before.tag.push_back(currTag);
}

void Dispatcher::tagSetBeforeTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    ENSURE_TAG_SETTINGS(tag);
    // Set curr to run before this tag
    tagSettings[currTag]->before.tag.push_back(tag);
    // And that tag to run after this tag
    tagSettings[tag]->after.tag.push_back(currTag);
}

void Dispatcher::systemAddTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_TAG_SETTINGS(tag);
    currSystem->tags.insert(tag);
}

void Dispatcher::systemSetAfterTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(currSystem);
    addTag(tag);
    // Set curr to run after this tag
    currSystem->settings->after.tag.push_back(tag);
    // And that tag to run before this system tag
    tagSettings[tag]->before.system.push_back(currSystem);
}

void Dispatcher::systemSetBeforeTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(currSystem);
    addTag(tag);
    // Set curr to run before this tag
    currSystem->settings->before.tag.push_back(tag);
    // And that tag to run after this system tag
    tagSettings[tag]->after.system.push_back(currSystem);
}

void Dispatcher::handleTagInheritance(std::shared_ptr<SystemSettings>& settings)
{
    for (auto& parentTag : settings->inherits)
    {
        auto& parentSettings = tagSettings[parentTag];
        handleTagInheritance(parentSettings);
        settings->copyFrom(parentSettings.get());
    }
    settings->inherits.clear();
}

void Dispatcher::compileChain()
{
    // Implement tag inheritance by copying configs from parent tags
    for (auto& [tag, settings] : tagSettings)
    {
        handleTagInheritance(settings);
    }

    // Implement system tag settings with custom settings
    for (System* system : pendingSystems)
    {
        for (const auto& tag : system->tags)
        {
            if (!system->settings)
            {
                system->settings = std::make_shared<SystemSettings>();
            }
            system->settings->copyFrom(tagSettings[tag].get());
        }
    }

    // Build the system chain
    std::vector<DFSNode> nodes;
    for (System* system : pendingSystems)
    {
        nodes.push_back(DFSNode{DFSNode::WHITE, system});
    }

    // Keep running while there are unvisited nodes
    std::vector<DFSNode>::iterator it;
    while ((it = std::find_if(nodes.begin(), nodes.end(),
                              [](const DFSNode& node) { return node.m == DFSNode::WHITE; })) != nodes.end())
    {
        if (dfsVisit(*it, nodes))
        {
            CUBOS_ERROR("Cycle detected in system chain! Ensure there are no circular dependencies!");
            return;
        }
    }

    // The algorithm expects nodes to be added to the head of a list, instead of the back. To save on
    // move operations, just reverse the final list for the same effect.
    std::reverse(systems.begin(), systems.end());

    CUBOS_INFO("Call chain completed successfully!");
    pendingSystems.clear();
    currSystem = nullptr;
    tagSettings.clear();
}

bool Dispatcher::dfsVisit(DFSNode& node, std::vector<DFSNode>& nodes)
{
    switch (node.m)
    {
    case DFSNode::BLACK: // Node has been fully visited already. Nothing else to do.
        return false;
    case DFSNode::GRAY: // Node is being processed. This means there's a cycle.
        return true;
    case DFSNode::WHITE: // Node is unexplored.
    {
        node.m = DFSNode::GRAY;
        System* systemInfo = node.s;
        if (systemInfo->settings)
        {
            // Visit tags first
            for (std::string tag : systemInfo->settings->before.tag)
            {
                for (auto it = nodes.begin(); it != nodes.end(); it++)
                {
                    if (std::find(it->s->tags.begin(), it->s->tags.end(), tag) != it->s->tags.end())
                    {
                        if (dfsVisit(*it, nodes))
                        {
                            return true;
                        }
                    }
                }
            }
            // Now visit systems
            for (System* system : systemInfo->settings->before.system)
            {
                SystemSettings* settings = system->settings.get();
                for (auto it = nodes.begin(); it != nodes.end(); it++)
                {
                    if (it->s->settings.get() == settings)
                    {
                        if (dfsVisit(*it, nodes))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        // All children nodes were visited; mark this node as complete
        node.m = DFSNode::BLACK;
        systems.push_back(systemInfo);
        return false;
    }
    }
    return false;
}

void Dispatcher::callSystems(World& world, CommandBuffer& cmds)
{
    // If the systems haven't been prepared yet, do so now.
    // We can't multi-thread this as the systems require exclusive access to the world to prepare.
    if (!this->prepared)
    {
        for (auto& system : systems)
        {
            system->system->prepare(world);
        }

        for (auto& condition : conditions)
        {
            condition->prepare(world);
        }

        this->prepared = true;
    }

    // Clear conditions bitmasks
    runConditions.reset();
    retConditions.reset();

    for (auto& system : systems)
    {
        // Query for conditions
        bool canRun = true;
        auto conditionsMask = system->settings->conditions;
        std::size_t i = 0;
        while (conditionsMask.any())
        {
            if (conditionsMask.test(0))
            {
                // We have a condition, check if it has run already
                if (!runConditions.test(i))
                {
                    runConditions.set(i);
                    if (conditions[i]->call(world, cmds))
                    {
                        retConditions.set(i);
                    }
                }
                // Check if the condition returned true
                if (!retConditions.test(i))
                {
                    canRun = false;
                    break;
                }
            }

            i += 1;
            conditionsMask >>= 1;
        }

        if (canRun)
        {
            system->system->call(world, cmds);
        }

        // TODO: Check synchronization concerns when this gets multithreaded
        cmds.commit();
    }
}
