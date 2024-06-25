#include <cubos/core/ecs/system/registry.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::ecs::ConditionId;
using cubos::core::ecs::System;
using cubos::core::ecs::SystemId;
using cubos::core::ecs::SystemRegistry;

SystemRegistry::~SystemRegistry()
{
    for (auto* system : mSystems)
    {
        delete system;
    }

    for (auto* condition : mConditions)
    {
        delete condition;
    }
}

SystemId SystemRegistry::add(std::string name, System<void> system)
{
    mSystems.push_back(new System<void>(std::move(system)));
    mSystemNames.emplace_back(std::move(name));
    return {mSystems.size() - 1};
}

ConditionId SystemRegistry::add(std::string name, System<bool> condition)
{
    mConditions.push_back(new System<bool>(std::move(condition)));
    mConditionNames.emplace_back(std::move(name));
    return {mConditions.size() - 1};
}

void SystemRegistry::reset()
{
    for (auto* system : mSystems)
    {
        delete system;
    }

    for (auto* condition : mConditions)
    {
        delete condition;
    }

    mSystems.clear();
    mConditions.clear();
    mSystemNames.clear();
    mConditionNames.clear();
}

void SystemRegistry::remove(SystemId id)
{
    delete mSystems[id.inner];
    mSystems[id.inner] = nullptr;
}

void SystemRegistry::remove(ConditionId id)
{
    delete mConditions[id.inner];
    mConditions[id.inner] = nullptr;
}

const std::string& SystemRegistry::name(SystemId id) const
{
    return mSystemNames[id.inner];
}

const std::string& SystemRegistry::name(ConditionId id) const
{
    return mConditionNames[id.inner];
}

System<void>& SystemRegistry::system(SystemId id)
{
    CUBOS_ASSERT(mSystems[id.inner] != nullptr, "System was already removed");
    return *mSystems[id.inner];
}

System<bool>& SystemRegistry::condition(ConditionId id)
{
    CUBOS_ASSERT(mConditions[id.inner] != nullptr, "Condition was already removed");
    return *mConditions[id.inner];
}
