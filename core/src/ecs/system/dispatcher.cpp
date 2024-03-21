#include <algorithm>
#include <iterator>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/dispatcher.hpp>
#include <cubos/core/reflection/external/string.hpp>

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
}

void Dispatcher::removeTag(const std::string& tag)
{
    mTagSettings.erase(tag);

    for (auto& [name, settings] : mTagSettings)
    {
        std::erase_if(settings->after.tag, [&](const std::string& t) { return t == tag; });
        std::erase_if(settings->before.tag, [&](const std::string& t) { return t == tag; });
        std::erase_if(settings->inherits, [&](const std::string& t) { return t == tag; });
    }

    for (auto& system : mPendingSystems)
    {
        std::erase_if(system->tags, [&](const std::string& t) { return t == tag; });
        std::erase_if(system->settings->after.tag, [&](const std::string& t) { return t == tag; });
        std::erase_if(system->settings->before.tag, [&](const std::string& t) { return t == tag; });
        std::erase_if(system->settings->inherits, [&](const std::string& t) { return t == tag; });
    }
}

void Dispatcher::removeSystem(SystemId id)
{
    auto* system = mPendingSystems[id.inner];
    mPendingSystems[id.inner] = nullptr;

    for (auto& [name, settings] : mTagSettings)
    {
        std::erase_if(settings->after.system, [&](const System* s) { return s == system; });
        std::erase_if(settings->before.system, [&](const System* s) { return s == system; });
    }

    for (auto& otherSystem : mPendingSystems)
    {
        if (otherSystem != nullptr)
        {
            std::erase_if(otherSystem->settings->after.system, [&](const System* s) { return s == system; });
            std::erase_if(otherSystem->settings->before.system, [&](const System* s) { return s == system; });
        }
    }

    delete system;
}

void Dispatcher::addTag(const std::string& tag)
{
    CUBOS_ASSERT(!tag.starts_with('!'), "Tags starting with '!' are reserved for internal use");
    ENSURE_TAG_SETTINGS(tag);
    mCurrTag = tag;
    mCurrGroup = mMainStep;
}

void Dispatcher::addNegativeTag(const std::string& tag)
{
    CUBOS_ASSERT(!tag.starts_with('!'), "Tags starting with '!' are reserved for internal use");
    ENSURE_TAG_SETTINGS('!' + tag);
    mCurrTag = '!' + tag;
}

void Dispatcher::tagInheritTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    ENSURE_TAG_SETTINGS(tag);
    if (std::find(mTagSettings[mCurrTag]->inherits.begin(), mTagSettings[mCurrTag]->inherits.end(), tag) !=
        mTagSettings[mCurrTag]->inherits.end())
    {
        CUBOS_INFO("Tag already inherits from {}", tag);
    }
    mTagSettings[mCurrTag]->inherits.push_back(tag);
    mCurrGroup = mMainStep->findGroup(tag);
    if (mCurrGroup == nullptr)
    {
        mCurrGroup = mMainStep;
    }
}

void Dispatcher::tagSetAfterTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    ENSURE_TAG_SETTINGS(tag);
    // Set curr to run after this tag
    mTagSettings[mCurrTag]->after.tag.push_back(tag);
    // And that tag to run before this tag
    mTagSettings[tag]->before.tag.push_back(mCurrTag);
}

void Dispatcher::tagSetBeforeTag(const std::string& tag)
{
    ENSURE_CURR_TAG();
    ENSURE_TAG_SETTINGS(tag);
    // Set curr to run before this tag
    mTagSettings[mCurrTag]->before.tag.push_back(tag);
    // And that tag to run after this tag
    mTagSettings[tag]->after.tag.push_back(mCurrTag);
}

void Dispatcher::groupAddGroup()
{
    mCurrGroup = mCurrGroup->addGroupStep(mCurrTag, mCurrGroup);
}

void Dispatcher::systemAddTag(const std::string& tag)
{
    CUBOS_ASSERT(tag != "main", "Systems cannot be tagged main");
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(mCurrSystem);
    ENSURE_TAG_SETTINGS(tag);
    mCurrSystem->settings->inherits.push_back(tag);
    if (mMainStep->findGroup(tag) != nullptr)
    {
        this->systemAddGroup(tag);
    }
}

void Dispatcher::systemAddGroup(const std::string& grouptag)
{
    ENSURE_CURR_SYSTEM();
    CUBOS_ASSERT(mCurrSystem->groupTag == "main", "Systems can only be tagged with at most one repeating tag");
    mCurrSystem->groupTag = grouptag;
}

void Dispatcher::systemSetAfterTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(mCurrSystem);
    addTag(tag);
    // Set curr to run after this tag
    mCurrSystem->settings->after.tag.push_back(tag);
    // And that tag to run before this system tag
    mTagSettings[tag]->before.system.push_back(mCurrSystem);
}

void Dispatcher::systemSetBeforeTag(const std::string& tag)
{
    ENSURE_CURR_SYSTEM();
    ENSURE_SYSTEM_SETTINGS(mCurrSystem);
    addTag(tag);
    // Set curr to run before this tag
    mCurrSystem->settings->before.tag.push_back(tag);
    // And that tag to run after this system tag
    mTagSettings[tag]->after.system.push_back(mCurrSystem);
}

void Dispatcher::handleTagInheritance(std::shared_ptr<SystemSettings>& settings)
{
    for (auto& parentTag : settings->inherits)
    {
        auto& parentSettings = mCompiledTagSettings[parentTag];
        handleTagInheritance(parentSettings);
        settings->copyFrom(parentSettings.get());
    }
    settings->inherits.clear();
}

void Dispatcher::compileChain()
{
    mMainStep->clear();

    // Copy all tag settings to a new vector.
    for (auto& [tag, settings] : mTagSettings)
    {
    }

    // Implement tag inheritance by copying configs from parent tags
    for (auto& [tag, settings] : mCompiledTagSettings)
    {
        handleTagInheritance(settings);
    }

    // Implement system tag settings with custom settings
    for (System* system : mPendingSystems)
    {
        if (system == nullptr)
        {
            continue;
        }

        // Add negative tags when the system doesn't have the respective positive tags
        for (auto& [tag, settings] : mTagSettings)
        {
            if (tag.starts_with('!') && !system->tags.contains(tag.substr(1)))
            {
                system->tags.insert(tag);
            }
        }

        for (const auto& tag : system->tags)
        {
            if (!system->settings)
            {
                system->settings = std::make_shared<SystemSettings>();
            }
            system->settings->copyFrom(mTagSettings[tag].get());
        }
    }

    // Build the system chain
    std::vector<DFSNode> nodes;
    for (System* system : mPendingSystems)
    {
        if (system == nullptr)
        {
            continue;
        }

        nodes.push_back(DFSNode{DFSNode::WHITE, system, "", system->settings});
    }

    for (auto& [tag, settings] : mTagSettings)
    {
        nodes.push_back(DFSNode{DFSNode::WHITE, nullptr, tag, settings});
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

    CUBOS_DEBUG("Compiled system chain:");
    mMainStep->debug(0);

    mPendingSystems.clear();
    mCurrSystem = nullptr;
    mTagSettings.clear();
}

void Dispatcher::clear()
{
    mPendingSystems.clear();
    mTagSettings.clear();
    mConditions.clear();
    mRunConditions.reset();
    mRetConditions.reset();
    mCurrSystem = nullptr;
    mCurrTag = "";

    mMainStep = std::make_shared<GroupStep>("main", nullptr);
    mCurrGroup = mMainStep;
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
        if (node.settings)
        {
            // Visit tags first
            for (const std::string& tag : node.settings->before.tag)
            {
                for (auto it = nodes.begin(); it != nodes.end(); it++)
                {
                    if (it->s != nullptr)
                    {
                        if (std::find(it->s->tags.begin(), it->s->tags.end(), tag) != it->s->tags.end())
                        {
                            if (dfsVisit(*it, nodes))
                            {
                                return true;
                            }
                        }
                    }
                    else
                    {
                        if (tag == it->t || std::find(it->settings->inherits.begin(), it->settings->inherits.end(),
                                                      tag) != it->settings->inherits.end())
                        {
                            if (dfsVisit(*it, nodes))
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }

        // All children nodes were visited; mark this node as complete
        node.m = DFSNode::BLACK;
        if (node.s != nullptr)
        {
            mMainStep->queueSystem(node.s->groupTag, node.s);
        }
        return false;
    }
    }
    return false;
}

void Dispatcher::SystemStep::debug(std::size_t depth)
{
    std::string indent = std::string(depth * 2, ' ');
    std::string format = indent + "- system {}";
    CUBOS_DEBUG(format.c_str(), mSystem->name);
}

void Dispatcher::SystemStep::call(SystemContext& ctx, std::vector<ecs::System<bool>>& conditions,
                                  std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& runConditions,
                                  std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& retConditions)
{
    bool canRun = true;
    if (mSystem->settings != nullptr)
    {
        auto conditionsMask = mSystem->settings->conditions;
        std::size_t i = 0;
        while (conditionsMask.any())
        {
            if (conditionsMask.test(0))
            {
                // We have a condition, check if it has run already

                if (!runConditions.test(i))
                {
                    runConditions.set(i);
                    if (conditions[i].run(ctx))
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
    }

    if (canRun)
    {
        mSystem->system.run(ctx);
    }

    // TODO: Check synchronization concerns when this gets multithreaded
    ctx.cmdBuffer.commit();
}

void Dispatcher::GroupStep::debug(std::size_t depth)
{
    std::string indent = std::string(depth * 2, ' ');
    std::string format = indent + "- group {}:";
    CUBOS_DEBUG(format.c_str(), groupTag);
    for (auto& step : mSteps)
    {
        step->debug(depth + 1);
    }
}

void Dispatcher::GroupStep::call(SystemContext& ctx, std::vector<ecs::System<bool>>& conditions,
                                 std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& runConditions,
                                 std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& retConditions)
{
    bool canRun = true;
    while (canRun)
    {
        auto conditionsMask = this->conditions;
        std::size_t i = 0;
        while (conditionsMask.any())
        {
            if (conditionsMask.test(0))
            {
                // We have a condition, check if it has run already
                if (!runConditions.test(i))
                {
                    runConditions.set(i);
                }
                if ((conditions[i]).run(ctx))
                {
                    retConditions.set(i);
                }
                else
                {
                    retConditions.reset(i);
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
            for (auto& step : mSteps)
            {
                step->call(ctx, conditions, runConditions, retConditions);
            }
        }
        if (groupTag == "main")
        {
            canRun = false;
        }
    }
}

void Dispatcher::callSystems(SystemContext& ctx)
{
    // Clear conditions bitmasks
    mRunConditions.reset();
    mRetConditions.reset();

    mMainStep->call(ctx, mConditions, mRunConditions, mRetConditions);
}
