#include <cubos/core/ecs/dispatcher.hpp>

using namespace cubos::core::ecs;

// call systems from stagesOrder order
void Dispatcher::callSystems(World& world, ecs::Commands& cmds)
{
    logInfo("Calling stages by the following order:");
    for (auto& stageName : stagesOrder)
    {
        logInfo("{}", stageName);
    };
    // call systems
    for (auto& stageName : stagesOrder)
    {
        for (auto& system : stagesByName[stageName])
        {
            system->call(world, cmds);
        }
        cmds.commit();
    }
}

void Dispatcher::registerStage(std::string stage, std::string defaultStage)
{
    // check if stage already exists
    if (stagesByName.count(stage) > 0)
    {
        logWarning("Stage {} already exists", stage);
        return;
    }
    // check if default stage is set, if not create stage in end of stagesOrder vector
    if (defaultStage == "")
    {
        stagesOrder.push_back(stage);
    }
    // if default stage is set, add stage to vector after default vector if direction is AFTER or before if direction is
    // BEFORE
    else
    {
        auto it = std::find(stagesOrder.begin(), stagesOrder.end(), defaultStage);
        if (it == stagesOrder.end())
        {
            logWarning("Default stage {} not found", defaultStage);
            return;
        }
        if (direction == Direction::AFTER)
        {
            stagesOrder.insert(it + 1, stage);
        }
        else
        {
            stagesOrder.insert(it, stage);
        }
    }
    // add new vector of system wrapper to map
    stagesByName[stage] = std::vector<std::unique_ptr<AnySystemWrapper>>();
}

void Dispatcher::setDefaultStage(std::string stage, ecs::Dispatcher::Direction direction)
{
    // check if stage exists
    if (stagesByName.find(stage) == stagesByName.end())
    {
        logWarning("Stage {} not found", stage);
        return;
    }
    // set default stage and direction
    defaultStage = stage;
    this->direction = direction;
}

void Dispatcher::putStageBefore(std::string stage, std::string referenceStage)
{
    // check if stage exists
    if (stagesByName.find(stage) == stagesByName.end())
    {
        logWarning("Stage {} not found", stage);
        return;
    }
    // check if reference stage exists
    if (stagesByName.find(referenceStage) == stagesByName.end())
    {
        logWarning("Reference stage {} not found", referenceStage);
        return;
    }
    // find stage index in stagesOrder vector
    auto it = std::find(stagesOrder.begin(), stagesOrder.end(), stage);
    // find reference stage index in stagesOrder vector
    auto it2 = std::find(stagesOrder.begin(), stagesOrder.end(), referenceStage);
    // check if stage is before reference stage
    if (it < it2)
    {
        logWarning("Stage {} is already before reference stage {}", stage, referenceStage);
        return;
    }

    // check if stage is the same as reference stage
    if (it == it2)
    {
        logWarning("Stage {} is the same as reference stage {}", stage, referenceStage);
        return;
    }
    // remove stage from vector
    stagesOrder.erase(it);
    it2 = std::find(stagesOrder.begin(), stagesOrder.end(), referenceStage);
    // insert stage it before reference stage it2
    stagesOrder.insert(it2, stage);
}

void Dispatcher::putStageAfter(std::string stage, std::string referenceStage)
{
    // check if stage exists
    if (stagesByName.find(stage) == stagesByName.end())
    {
        logWarning("Stage {} not found", stage);
        return;
    }
    // check if reference stage exists
    if (stagesByName.find(referenceStage) == stagesByName.end())
    {
        logWarning("Reference stage {} not found", referenceStage);
        return;
    }
    // find stage index in stagesOrder vector
    auto it = std::find(stagesOrder.begin(), stagesOrder.end(), stage);
    // find reference stage index in stagesOrder vector
    auto it2 = std::find(stagesOrder.begin(), stagesOrder.end(), referenceStage);
    // check if stage is after reference stage
    if (it > it2)
    {
        logWarning("Stage {} is already after reference stage {}", stage, referenceStage);
        return;
    }

    // check if stage is the same as reference stage
    if (it == it2)
    {
        logWarning("Stage {} is the same as reference stage {}", stage, referenceStage);
        return;
    }
    // remove stage from vector
    stagesOrder.erase(it);
    it2 = std::find(stagesOrder.begin(), stagesOrder.end(), referenceStage);
    // insert stage it after reference stage it2
    stagesOrder.insert(it2 + 1, stage);
}
