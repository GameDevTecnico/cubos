#ifndef CUBOS_CORE_ECS_DISPATCHER_HPP
#define CUBOS_CORE_ECS_DISPATCHER_HPP

#include <cubos/core/ecs/system.hpp>
#include <cubos/core/log.hpp>

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace cubos::core::ecs
{
    /// @brief Dispatcher is a class that is used to register systems and call them all at once, where they will be
    /// executed in order of the stages they are in.
    class Dispatcher
    {
    public:
        // Direction enum
        enum class Direction
        {
            Before,
            After
        };

        /// Adds a system into a stage.
        /// @param system System to add.
        /// @param stage Stage to add the system in.
        template <typename F> void addSystem(F system, std::string stage);

        /// Calls all systems in order of the stages they are in.
        /// @param world World to call the systems in.
        void callSystems(World& world, Commands& cmds);

        /// Sets the default stage and the direction new stages will be added in.
        /// @param stage The stage to set as default.
        /// @param direction Direction to add new stages in.
        void setDefaultStage(std::string stage, ecs::Dispatcher::Direction direction);

        /// Puts a stage before another stage.
        /// @param stage Stage to put before another stage.
        /// @param before Stage to put the stage before.
        void putStageBefore(std::string stage, std::string referenceStage);

        /// Puts a stage after another stage.
        /// @param stage Stage to put after another stage.
        /// @param after Stage to put the stage after.
        void putStageAfter(std::string stage, std::string referenceStage);

        /// Puts a stage next to another stage.
        /// @param stage Stage to put next to another stage.
        /// @param referenceStage Stage to put the stage next to.
        /// @param direction Direction to put the stage in.
        void putStage(std::string stage, std::string referenceStage, Direction direction);

    private:
        std::vector<std::string> stagesOrder; ///< Order in which stages are dispatched.
        std::map<std::string, std::vector<std::unique_ptr<AnySystemWrapper<void>>>>
            stagesByName;           ///< Maps names to stages.
        std::string defaultStage;   ///< The stage that new stages are put after/before.
        Direction defaultDirection; ///< The direction new stages are put in relation to the default stage.
    };

    template <typename F> void Dispatcher::addSystem(F system, std::string stage)
    {
        // Register stage if it doesn't exist.
        if (this->stagesByName.find(stage) == this->stagesByName.end())
        {
            this->putStage(stage, this->defaultStage, this->defaultDirection);
        }

        // Wrap the system and put it in its stage.
        auto systemWrapper = std::make_unique<SystemWrapper<F>>(system);
        for (auto& system : this->stagesByName[stage])
        {
            if (system->info().compatible(systemWrapper->info()))
            {
                // TODO: explain whats wrong, e.g., both systems are trying to access the same component mutably
                CUBOS_CRITICAL("Could not add system to stage '{}' because it is incompatible "
                               "with another system in the same stage.",
                               stage);
                return;
            }
        }
        this->stagesByName[stage].push_back(std::move(systemWrapper));
        CUBOS_INFO("Added system to stage '{}'", stage);
    }
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_DISPATCHER_HPP
