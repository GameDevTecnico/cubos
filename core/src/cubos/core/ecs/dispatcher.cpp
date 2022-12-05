#include <cubos/core/ecs/dispatcher.hpp>

using namespace cubos::core::ecs;

void Dispatcher::callSystems(World& world, Commands& cmds)
{
    for (auto& stageName : this->stagesOrder)
    {
        CUBOS_TRACE("Dispatching stage '{}'", stageName);

        for (auto& system : this->stagesByName[stageName])
        {
            system->call(world, cmds);
        }

        cmds.commit();
    }
}

void Dispatcher::setDefaultStage(std::string stage, Direction direction)
{
    // Check if stage exists.
    if (this->stagesByName.find(stage) == this->stagesByName.end())
    {
        this->putStage(stage, this->defaultStage, this->defaultDirection);
    }

    this->defaultStage = stage;
    this->defaultDirection = direction;
}

void Dispatcher::putStageBefore(std::string stage, std::string referenceStage)
{
    this->putStage(stage, referenceStage, Direction::Before);
}

void Dispatcher::putStageAfter(std::string stage, std::string referenceStage)
{
    this->putStage(stage, referenceStage, Direction::After);
}

void Dispatcher::putStage(std::string stage, std::string referenceStage, Direction direction)
{
    // Check if the stage is valid.
    if (stage == "")
    {
        CUBOS_CRITICAL("Stage name must not be empty");
        abort();
    }
    // Check if they're the same stage.
    else if (stage == referenceStage)
    {
        CUBOS_CRITICAL("Cannot put the stage '{}' after/before itself", stage);
        abort();
    }

    // Get the iterators for both stages in the order vector.
    auto it = std::find(this->stagesOrder.begin(), this->stagesOrder.end(), stage);
    auto refIt = std::find(this->stagesOrder.begin(), this->stagesOrder.end(), referenceStage);

    // Check if the reference stage exists
    if (refIt == this->stagesOrder.end() && !referenceStage.empty())
    {
        CUBOS_CRITICAL("Stage '{}' not found", referenceStage);
        abort();
    }

    // Remove the stage from the order vector and reinsert it before the reference stage.
    if (it != this->stagesOrder.end())
    {
        this->stagesOrder.erase(it);
    }

    if (referenceStage.empty() || direction == Direction::Before)
    {
        this->stagesOrder.insert(refIt, stage);
    }
    else
    {
        this->stagesOrder.insert(refIt + 1, stage);
    }

    // Check if we need to initialize the new stage.
    if (this->stagesByName.find(stage) == this->stagesByName.end())
    {
        this->stagesByName[stage] = std::vector<std::unique_ptr<AnySystemWrapper>>();
    }
}
