#include <cubos/core/ecs/dispatcher.hpp>

using namespace cubos::core::ecs;

void Dispatcher::addTag(const std::string& tag)
{
    if(tagSettings.find("tag") == tagSettings.end())
    {
        tagSettings[tag] = std::make_shared<SystemSettings>();
    }
}

void Dispatcher::inheritTag(const std::string& tag, const std::string& other)
{
    addTag(tag);
    auto settings = tagSettings[tag];
    if(std::find(settings->inherits.begin(), settings->inherits.end(), other) != settings->inherits.end())
    {
        CUBOS_INFO("Tag '{}' already inherits from '{}'", tag, other);
    }
    settings->inherits.push_back(other);
}

void Dispatcher::setTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS();
    addTag(tag);
    currSystem->settings = tagSettings[tag];
}

void Dispatcher::setAfter(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS();
    addTag(tag);
    currSystem->settings->after.tag.push_back(tag);
}

void Dispatcher::setBefore(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS();
    addTag(tag);
    currSystem->settings->before.tag.push_back(tag);
}

void Dispatcher::compileChain()
{
    // Build the system chain
    std::vector<DFSNode> nodes;
    for(auto& system : pendingSystems)
    {
        nodes.push_back(DFSNode{DFSNode::WHITE, &system});
    }

    // Keep running while there are unvisited nodes
    std::vector<DFSNode>::iterator it;
    while((it = std::find_if(nodes.begin(), nodes.end(), [](const DFSNode& node) { return node.m == DFSNode::WHITE; })) != nodes.end())
    {
        if(dfsVisit(*it))
        {
            CUBOS_ERROR("Cycle detected in system chain! Ensure there are no circular dependencies!");
            return;
        }
    }
}

bool Dispatcher::dfsVisit(DFSNode& node)
{
    switch(node.m)
    {
        case DFSNode::BLACK:
            return false;
        case DFSNode::GRAY:
            return true;
        case DFSNode::WHITE:
        {
            node.m = DFSNode::GRAY;
            System* system = node.s;
            // TODO: Implement
            return false;
        }
    }
}

void Dispatcher::callSystems(World& world, Commands& cmds)
{
    for(auto it = systems.begin(); it != systems.end(); it++)
    {
        it->system->call(world, cmds);
        // TODO: Check synchronization concerns when this gets multithreaded
        cmds.commit();
    }
    /*for (auto& stageName : this->stagesOrder)
    {
        CUBOS_TRACE("Dispatching stage '{}'", stageName);

        for (auto& system : this->stagesByName[stageName])
        {
            system->call(world, cmds);
        }

        cmds.commit();
    }*/
}
