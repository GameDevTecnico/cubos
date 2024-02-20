/// @file
/// @brief Class @ref cubos::core::ecs::Dispatcher.
/// @ingroup core-ecs-system

#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/log.hpp>

#define ENSURE_CURR_SYSTEM()                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!mCurrSystem)                                                                                              \
        {                                                                                                              \
            CUBOS_ERROR("No system currently selected!");                                                              \
            return;                                                                                                    \
        }                                                                                                              \
    } while (false)
#define ENSURE_CURR_TAG()                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mCurrTag.empty())                                                                                          \
        {                                                                                                              \
            CUBOS_ERROR("No tag currently selected!");                                                                 \
            return;                                                                                                    \
        }                                                                                                              \
    } while (false)
#define ENSURE_SYSTEM_SETTINGS(obj)                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(obj)->settings)                                                                                          \
        {                                                                                                              \
            (obj)->settings = std::make_shared<SystemSettings>();                                                      \
        }                                                                                                              \
    } while (false)
#define ENSURE_TAG_SETTINGS(tag)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mTagSettings.find(tag) == mTagSettings.end())                                                              \
        {                                                                                                              \
            mTagSettings[tag] = std::make_shared<SystemSettings>();                                                    \
        }                                                                                                              \
    } while (false)

namespace cubos::core::ecs
{
    /// @brief Used to add systems and relations between them and then dispatch them all at once.
    /// @ingroup core-ecs-system
    class Dispatcher
    {
    public:
        ~Dispatcher();

        /// @brief Adds a tag, and sets it as the current tag for further settings.
        /// @param tag Tag to add.
        void addTag(const std::string& tag);

        /// @brief Makes the current tag inherit the settings of another tag.
        /// @param tag Tag to inherit from.
        void tagInheritTag(const std::string& tag);

        /// @brief Makes the current tag run after the given tag.
        /// @param tag Tag to run after.
        void tagSetAfterTag(const std::string& tag);

        /// @brief Makes the current tag run before the given tag.
        /// @param tag Tag to run before.
        void tagSetBeforeTag(const std::string& tag);

        /// @brief Adds a condition to the current tag.
        /// @param condition Condition to add.
        void tagAddCondition(ecs::System<bool> condition);

        /// @brief Adds a repeat condition to the current tag.
        /// @param condition Condition to add.
        void tagRepeatWhile(ecs::System<bool> condition);

        /// @brief Adds a (sub)group to the current group.
        void groupAddGroup();

        /// @brief Adds a system, and sets it as the current system for further configuration.
        /// @param name System name.
        /// @param system System to add.
        void addSystem(std::string name, ecs::System<void> system);

        /// @brief Sets the tag for the current system.
        /// @param tag Tag to run under.
        void systemAddTag(const std::string& tag);

        /// @brief Links a system to a certain group.
        /// @param grouptag Tag of the group.
        void systemAddGroup(const std::string& grouptag);

        /// @brief Sets the current system to run after the tag.
        /// @param tag Tag to run after.
        void systemSetAfterTag(const std::string& tag);

        /// @brief Sets the current system to run before the tag.
        /// @param tag Tag to run before.
        void systemSetBeforeTag(const std::string& tag);

        /// @brief Adds a condition to the current system.
        /// @param condition Condition.
        void systemAddCondition(ecs::System<bool> condition);

        /// @brief Compiles the call chain. Required before @ref callSystems() can be called.
        ///
        /// Takes all pending systems and determines their execution order.
        void compileChain();

        /// @brief Calls all systems in the compiled call chain. @ref compileChain() must be called
        /// prior to this.
        /// @param cmds Command buffer.
        void callSystems(CommandBuffer& cmds);

        struct Dependency;
        struct SystemSettings;
        struct System;

        /// @brief Internal class to specify system dependencies
        struct Dependency
        {
            std::vector<std::string> tag;
            std::vector<System*> system;
        };

        /// @brief Internal class with settings pertaining to system/tag execution
        struct SystemSettings
        {
            void copyFrom(const SystemSettings* other);

            Dependency before, after;
            std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> conditions;
            // TODO: Add threading modes, etc...
            std::vector<std::string> inherits;
        };

        /// @brief Internal class to handle tag settings
        struct System
        {
            std::string name;
            std::shared_ptr<SystemSettings> settings;
            ecs::System<void> system;
            std::unordered_set<std::string> tags;
            std::string groupTag = "main";
        };

        class Step
        {
        public:
            virtual ~Step() = default;
            virtual void debug(std::size_t depth) = 0;
            virtual void call(CommandBuffer& cmds, std::vector<ecs::System<bool>>& conditions,
                              std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& runConditions,
                              std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& retConditions) = 0;
        };

        class SystemStep : public Step
        {
        public:
            SystemStep(System* system)
                : mSystem(system)
            {
            }

            void debug(std::size_t depth) override;
            void call(CommandBuffer& cmds, std::vector<ecs::System<bool>>& conditions,
                      std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& runConditions,
                      std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& retConditions) override;

            System* getSystem()
            {
                return mSystem;
            }

        private:
            System* mSystem;
        };

        class GroupStep : public Step, public std::enable_shared_from_this<Step>
        {
        public:
            GroupStep(std::string grouptag, std::shared_ptr<GroupStep> parentStep)
                : groupTag(std::move(grouptag))
                , mParentStep(std::move(parentStep))
            {
            }

            void debug(std::size_t depth) override;
            void call(CommandBuffer& cmds, std::vector<ecs::System<bool>>& conditions,
                      std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& runConditions,
                      std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>& retConditions) override;

            void addSystemStep(System* system)
            {
                mSteps.insert(mSteps.begin(), std::make_shared<SystemStep>(system));
            }

            std::shared_ptr<GroupStep> addGroupStep(const std::string& grouptag,
                                                    const std::shared_ptr<GroupStep>& parent)
            {
                std::shared_ptr<GroupStep> newStep = std::make_shared<GroupStep>(grouptag, parent);
                mSteps.insert(mSteps.begin(), newStep);
                return std::dynamic_pointer_cast<GroupStep>(mSteps[0]);
            }

            void moveToFront(const std::shared_ptr<Step>& step)
            {
                auto it = std::find(mSteps.begin(), mSteps.end(), step);
                if (it != mSteps.end())
                {
                    std::rotate(mSteps.begin(), it, it + 1);
                }
            }

            std::shared_ptr<GroupStep> findGroup(const std::string& tag)
            {
                if (tag == groupTag)
                {
                    return std::dynamic_pointer_cast<GroupStep>(shared_from_this());
                }

                for (const auto& step : this->mSteps)
                {
                    if (auto groupStepPtr = std::dynamic_pointer_cast<GroupStep>(step))
                    {
                        return groupStepPtr->findGroup(tag);
                    }
                }

                return nullptr;
            }

            void queueSystem(const std::string& grouptag, System* system)
            {
                if (grouptag == this->groupTag)
                {
                    addSystemStep(system);

                    if (this->groupTag != "main")
                    {
                        this->mParentStep->moveToFront(shared_from_this());
                    }
                    return;
                }

                for (const auto& step : this->mSteps)
                {
                    if (auto groupStepPtr = std::dynamic_pointer_cast<GroupStep>(step))
                    {
                        groupStepPtr->queueSystem(grouptag, system);
                    }
                }
            }

            const std::string& getGroupTag()
            {
                return groupTag;
            }
            std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> conditions;

        private:
            const std::string groupTag;
            std::shared_ptr<GroupStep> mParentStep;
            std::vector<std::shared_ptr<Step>> mSteps;
        };

    private:
        /// @brief Internal class used to implement a DFS algorithm for call chain compilation
        struct DFSNode
        {
            enum
            {
                WHITE,
                GRAY,
                BLACK
            } m;
            System* s;
            std::string t;
            std::shared_ptr<SystemSettings> settings;
        };

        /// @brief Visits a DFSNode to create a topological order.
        /// @param node Node to visit.
        /// @param nodes Array of DFSNodes.
        /// @return True if a cycle was detected, false if otherwise.
        bool dfsVisit(DFSNode& node, std::vector<DFSNode>& nodes);

        /// @brief Copies settings from inherited tags to this system, recursively solving nested
        /// inheritance.
        /// @param settings Settings to handle inheritance for.
        void handleTagInheritance(std::shared_ptr<SystemSettings>& settings);

        /// @brief Assign a condition a bit in the condition bitset, and returns that assigned bit.
        /// @param condition Condition to assign a bit for.
        /// @return Assigned bit.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> assignConditionBit(ecs::System<bool> condition);

        // Variables for holding information before call chain is compiled.

        std::vector<System*> mPendingSystems;                                ///< All systems.
        std::map<std::string, std::shared_ptr<SystemSettings>> mTagSettings; ///< All tags.
        std::vector<ecs::System<bool>> mConditions;                          ///< All conditions.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>
            mRunConditions; ///< Bitset of conditions that run in this iteration.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> mRetConditions; ///< Bitset of conditions return values.
        System* mCurrSystem;                                              ///< Last set system, for changing settings.
        std::string mCurrTag;                                             ///< Last set tag, for changing settings.

        // Variables for holding information after call chain is compiled.

        std::vector<System*> mSystems; ///< Compiled order of running systems.
        std::shared_ptr<GroupStep> mMainStep = std::make_shared<GroupStep>("main", nullptr);
        std::shared_ptr<GroupStep> mCurrGroup = mMainStep;
    };

    inline void Dispatcher::tagAddCondition(ecs::System<bool> condition)
    {
        ENSURE_CURR_TAG();
        auto bit = assignConditionBit(std::move(condition));
        mTagSettings[mCurrTag]->conditions |= bit;
    }

    inline void Dispatcher::tagRepeatWhile(ecs::System<bool> condition)
    {
        ENSURE_CURR_TAG();
        groupAddGroup();
        auto bit = assignConditionBit(std::move(condition));
        mCurrGroup->conditions |= bit;
    }

    inline void Dispatcher::addSystem(std::string name, ecs::System<void> system)
    {
        // Wrap the system and put it in the pending queue
        mPendingSystems.push_back(new System{std::move(name), nullptr, std::move(system), {}});
        mCurrSystem = mPendingSystems.back();
    }

    inline void Dispatcher::systemAddCondition(ecs::System<bool> condition)
    {
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS(mCurrSystem);
        auto bit = assignConditionBit(std::move(condition));
        mCurrSystem->settings->conditions |= bit;
    }

    inline std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> Dispatcher::assignConditionBit(ecs::System<bool> condition)
    {
        // If we already reached the condition limit, exit
        if (mConditions.size() >= CUBOS_CORE_DISPATCHER_MAX_CONDITIONS)
        {
            CUBOS_ERROR("Condition limit ({}) was reached!", CUBOS_CORE_DISPATCHER_MAX_CONDITIONS);
            abort();
        }

        // Otherwise, add it to the conditions list
        mConditions.push_back(std::move(condition));
        // And return the bit it has
        return std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>(1) << (mConditions.size() - 1);
    }
} // namespace cubos::core::ecs
