#include <cubos/core/ecs/dispatcher.hpp>

#include <algorithm>

using namespace cubos::core::ecs;

void Dispatcher::addTag(const std::string& tag)
{
    if (tagSettings.find(tag) == tagSettings.end())
    {
        tagSettings[tag] = std::make_shared<SystemSettings>();
        currTagSettings = tagSettings[tag].get();
    }
}

void Dispatcher::tagInheritTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    addTag(tag);
    if (std::find(currTagSettings->inherits.begin(), currTagSettings->inherits.end(), tag) !=
        currTagSettings->inherits.end())
    {
        CUBOS_INFO("Tag already inherits from '{}'", tag);
    }
    currTagSettings->inherits.push_back(tag);
}

void Dispatcher::tagSetAfterTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    addTag(tag);
    // Set curr to run after this tag
    currTagSettings->after.tag.push_back(tag);
    // And, if it exists, this tag to run before curr's tag
    auto settings = currTagSettings;
    std::map<std::string, std::shared_ptr<SystemSettings>>::iterator it;
    if ((it = std::find_if(tagSettings.begin(), tagSettings.end(),
                           [&settings](auto it) { return it.second.get() == settings; })) != tagSettings.end())
    {
        tagSettings[tag]->before.tag.push_back(it->first);
    }
}

void Dispatcher::tagSetBeforeTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    addTag(tag);
    // Set curr to run before this tag
    currTagSettings->before.tag.push_back(tag);
    // And, if it exists, this tag to run after curr's tag
    auto settings = currTagSettings;
    std::map<std::string, std::shared_ptr<SystemSettings>>::iterator it;
    if ((it = std::find_if(tagSettings.begin(), tagSettings.end(),
                           [&settings](auto it) { return it.second.get() == settings; })) != tagSettings.end())
    {
        tagSettings[tag]->after.tag.push_back(it->first);
    }
}

void Dispatcher::systemSetTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(currSystem);
    addTag(tag);
    currSystem->settings = tagSettings[tag];
}

void Dispatcher::systemSetAfterTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(currSystem);
    addTag(tag);
    // Set curr to run after this tag
    currSystem->settings->after.tag.push_back(tag);
    // And, if it exists, this tag to run before curr's tag
    auto* settings = currSystem->settings.get();
    std::map<std::string, std::shared_ptr<SystemSettings>>::iterator it;
    if ((it = std::find_if(tagSettings.begin(), tagSettings.end(),
                           [&settings](auto it) { return it.second.get() == settings; })) != tagSettings.end())
    {
        tagSettings[tag]->before.tag.push_back(it->first);
    }
}

void Dispatcher::systemSetBeforeTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(currSystem);
    addTag(tag);
    // Set curr to run before this tag
    currSystem->settings->before.tag.push_back(tag);
    // And, if it exists, this tag to run after curr's tag
    auto* settings = currSystem->settings.get();
    std::map<std::string, std::shared_ptr<SystemSettings>>::iterator it;
    if ((it = std::find_if(tagSettings.begin(), tagSettings.end(),
                           [&settings](auto it) { return it.second.get() == settings; })) != tagSettings.end())
    {
        tagSettings[tag]->after.tag.push_back(it->first);
    }
}

void Dispatcher::compileChain()
{
    // Build the system chain
    std::vector<DFSNode> nodes;
    for (auto& system : pendingSystems)
    {
        nodes.push_back(DFSNode{DFSNode::WHITE, &system});
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
                SystemSettings* settings = tagSettings[tag].get();
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
        systems.push_back(*systemInfo);
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
            system.system->prepare(world);
        this->prepared = true;
    }

    for (auto it = systems.begin(); it != systems.end(); it++)
    {
        it->system->call(world, cmds);
        // TODO: Check synchronization concerns when this gets multithreaded
        cmds.commit();
    }
}
