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
        /// @param func Condition to add.
        void tagAddCondition(std::shared_ptr<AnySystemWrapper<bool>> func);

        /// @brief Adds a system, and sets it as the current system for further configuration.
        /// @param func System to add.
        void addSystem(std::shared_ptr<AnySystemWrapper<void>> func);

        /// @brief Sets the tag for the current system.
        /// @param tag Tag to run under.
        void systemAddTag(const std::string& tag);

        /// @brief Sets the current system to run after the tag.
        /// @param tag Tag to run after.
        void systemSetAfterTag(const std::string& tag);

        /// @brief Sets the current system to run before the tag.
        /// @param tag Tag to run before.
        void systemSetBeforeTag(const std::string& tag);

        /// @brief Adds a condition to the current system.
        /// @param func Condition.
        void systemAddCondition(std::shared_ptr<AnySystemWrapper<bool>> func);

        /// @brief Compiles the call chain. Required before @ref callSystems() can be called.
        ///
        /// Takes all pending systems and determines their execution order.
        void compileChain();

        /// @brief Calls all systems in the compiled call chain. @ref compileChain() must be called
        /// prior to this.
        /// @param world World to call the systems in.
        /// @param cmds Command buffer.
        void callSystems(World& world, CommandBuffer& cmds);

    private:
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
            std::shared_ptr<SystemSettings> settings;
            std::shared_ptr<AnySystemWrapper<void>> system;
            std::unordered_set<std::string> tags;
        };

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
        /// @param func Condition to assign a bit for.
        /// @return Assigned bit.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> assignConditionBit(
            std::shared_ptr<AnySystemWrapper<bool>> func);

        // Variables for holding information before call chain is compiled.

        std::vector<System*> mPendingSystems;                                ///< All systems.
        std::map<std::string, std::shared_ptr<SystemSettings>> mTagSettings; ///< All tags.
        std::vector<std::shared_ptr<AnySystemWrapper<bool>>> mConditions;    ///< All conditions.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>
            mRunConditions; ///< Bitset of conditions that run in this iteration.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> mRetConditions; ///< Bitset of conditions return values.
        System* mCurrSystem;                                              ///< Last set system, for changing settings.
        std::string mCurrTag;                                             ///< Last set tag, for changing settings.

        // Variables for holding information after call chain is compiled.

        std::vector<System*> mSystems; ///< Compiled order of running systems.
        bool mPrepared = false;        ///< Whether the systems are prepared for execution.
    };

    inline void Dispatcher::tagAddCondition(std::shared_ptr<AnySystemWrapper<bool>> func)
    {
        ENSURE_CURR_TAG();
        auto bit = assignConditionBit(std::move(func));
        mTagSettings[mCurrTag]->conditions |= bit;
    }

    inline void Dispatcher::addSystem(std::shared_ptr<AnySystemWrapper<void>> func)
    {
        // Wrap the system and put it in the pending queue
        auto* system = new System{nullptr, std::move(func), {}};
        mPendingSystems.push_back(system);
        mCurrSystem = mPendingSystems.back();
    }

    inline void Dispatcher::systemAddCondition(std::shared_ptr<AnySystemWrapper<bool>> func)
    {
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS(mCurrSystem);
        auto bit = assignConditionBit(std::move(func));
        mCurrSystem->settings->conditions |= bit;
    }

    inline std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> Dispatcher::assignConditionBit(
        std::shared_ptr<AnySystemWrapper<bool>> func)
    {
        // If we already reached the condition limit, exit
        if (mConditions.size() >= CUBOS_CORE_DISPATCHER_MAX_CONDITIONS)
        {
            CUBOS_ERROR("Condition limit ({}) was reached!", CUBOS_CORE_DISPATCHER_MAX_CONDITIONS);
            abort();
        }

        // Otherwise, add it to the conditions list
        mConditions.push_back(func);
        // And return the bit it has
        return std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>(1) << (mConditions.size() - 1);
    }
} // namespace cubos::core::ecs
