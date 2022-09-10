#ifndef CUBOS_CORE_ECS_DISPATCHER_HPP
#define CUBOS_CORE_ECS_DISPATCHER_HPP

#include <cubos/core/ecs/system.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <cubos/core/log.hpp>

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
            BEFORE,
            AFTER
        };

        /// Registers a system in a stage
        /// @param system System to register
        /// @param stage Stage to register the system in
        template <typename F> void registerSystem(F system, std::string stage);

        /// Calls all systems in order of the stages they are in
        /// @param world World to call the systems in
        void callSystems(World& world);

        /// Sets the default stage and the direction new stages will be added in
        /// @param stage the stage to set as default
        /// @param direction Direction to add new stages in
        void setDefaultStage(std::string stage, ecs::Dispatcher::Direction direction);

        /// Puts a stage before another stage
        /// @param stage Stage to put before another stage
        /// @param before Stage to put the stage before
        void putStageBefore(std::string stage, std::string referenceStage);

        /// Puts a stage after another stage
        /// @param stage Stage to put after another stage
        /// @param after Stage to put the stage after
        void putStageAfter(std::string stage, std::string referenceStage);

    private:
        /// Registers a new stage
        /// @param stage the stage to register
        /// @param defaultStage the default stage of the dispatcher
        void registerStage(std::string stage, std::string defaultStage);

        std::vector<std::string> stagesOrder;                                               ///< Vector of stages order
        std::map<std::string, std::vector<std::unique_ptr<AnySystemWrapper>>> stagesByName; ///< Map of stages by name
        std::string defaultStage;                                                           ///< Default stage
        ecs::Dispatcher::Direction direction; ///< Direction of default stage
    };

    template <typename F> void Dispatcher::registerSystem(F system, std::string stage)
    {
        // Register stage if it doesn't exist
        if (stagesByName.find(stage) == stagesByName.end())
        {
            registerStage(stage, defaultStage);
        }

        // Create system wrapper
        auto systemWrapper = std::make_unique<SystemWrapper<F>>(system);

        // Add system to stage
        stagesByName[stage].push_back(std::move(systemWrapper));
        logInfo("Added system to: {}", stage);
    }
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_DISPATCHER_HPP