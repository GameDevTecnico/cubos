#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <cubos/core/ecs/system.hpp>
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
    /// @brief Dispatcher is a class that is used to register systems and call them all at once, where they will be
    /// executed in order of the stages they are in.
    class Dispatcher
    {
    public:
        ~Dispatcher();

        /// Adds a tag, and sets it as the current tag for further
        /// settings.
        /// @param tag Tag to add.
        void addTag(const std::string& tag);

        /// Makes the current tag inherit the settings of another tag.
        /// @param tag Tag to inherit from.
        void tagInheritTag(const std::string& tag);

        /// Makes the current tag run after the given tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag Tag to run after.
        void tagSetAfterTag(const std::string& tag);

        /// Makes the current tag run before the given tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag Tag to run before.
        void tagSetBeforeTag(const std::string& tag);

        /// Adds a condition to the current tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param func The condition.
        template <typename F>
        void tagAddCondition(F func);

        /// Adds a system, and sets it as the current system for further
        /// settings.
        /// @param func System to add.
        template <typename F>
        void addSystem(F func);

        /// Sets the tag for the current system.
        /// @param tag The tag to run under.
        void systemAddTag(const std::string& tag);

        /// Sets the current system to run after the tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run after.
        void systemSetAfterTag(const std::string& tag);

        /// Sets the current system to run after a given system.
        /// The specified system must exist.
        /// @param func The system to run after.
        template <typename F>
        void systemSetAfterSystem(F func);

        /// Sets the current system to run before the tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run before.
        void systemSetBeforeTag(const std::string& tag);

        /// Sets the current system to run before the system.
        /// The specified system must exist.
        /// @param func The system to run before.
        template <typename F>
        void systemSetBeforeSystem(F func);

        /// Adds a condition to the current system.
        /// The specified system must exist.
        /// @param func The condition.
        template <typename F>
        void systemAddCondition(F func);

        /// Compiles a call chain. This takes all pending systems and
        /// determines their execution order. This is required before
        /// systems can be run.
        void compileChain();

        /// Calls all systems in order of the stages they are in.
        /// The call chain must be compiled successfully prior, otherwise
        /// this function won't run anything.
        /// @param world World to call the systems in.
        /// @param cmds Command buffer.
        void callSystems(World& world, CommandBuffer& cmds);

    private:
        struct Dependency;
        struct SystemSettings;
        struct System;

        /// Internal class to specify system dependencies
        struct Dependency
        {
            std::vector<std::string> tag;
            std::vector<System*> system;
        };

        /// Internal class with settings pertaining to system/tag execution
        struct SystemSettings
        {
            void copyFrom(const SystemSettings* other);

            Dependency before, after;
            std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> conditions;
            // TODO: Add run conditions, threading modes, etc...
            std::vector<std::string> inherits;
        };

        /// Internal class to handle tag settings
        struct System
        {
            std::shared_ptr<SystemSettings> settings;
            std::shared_ptr<AnySystemWrapper<void>> system;
            std::unordered_set<std::string> tags;
        };

        /// Internal class used to implement a DFS algorithm for call chain compilation
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

        /// Visits a DFSNode to create a topological order.
        /// This is used internally during call chain compilation.
        /// @param node The node to visit.
        /// @param nodes Array of DFSNodes.
        /// @return True if a cycle was detected, false if otherwise.
        bool dfsVisit(DFSNode& node, std::vector<DFSNode>& nodes);

        /// Copies settings from inherited tags to this system, recursively
        /// solving nested inheritance.
        /// @param settings Settings to handle inheritance for.
        void handleTagInheritance(std::shared_ptr<SystemSettings>& settings);

        /// Assign a condition a bit in the condition bitset,
        /// and returns that assigned bit.
        /// @param func Condition to assign a bit for.
        /// @return The assigned bit.
        template <typename F>
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> assignConditionBit(F func);

        /// Variables for holding information before call chain is compiled.
        std::vector<System*> mPendingSystems;                                ///< All systems.
        std::map<std::string, std::shared_ptr<SystemSettings>> mTagSettings; ///< All tags.
        std::vector<std::shared_ptr<AnySystemWrapper<bool>>> mConditions;    ///< All conditions.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>
            mRunConditions; ///< Bitset of conditions that run in this iteration.
        std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> mRetConditions; ///< Bitset of conditions return values.
        System* mCurrSystem;                                              ///< Last set system, for changing settings.
        std::string mCurrTag;                                             ///< Last set tag, for changing settings.

        /// Variables for holding information after call chain is compiled.
        std::vector<System*> mSystems; ///< Compiled order of running systems.
        bool mPrepared = false;        ///< Whether the systems are prepared for execution.
    };

    template <typename F>
    void Dispatcher::tagAddCondition(F func)
    {
        ENSURE_CURR_TAG();
        auto bit = assignConditionBit(func);
        mTagSettings[mCurrTag]->conditions |= bit;
    }

    template <typename F>
    void Dispatcher::addSystem(F func)
    {
        // Wrap the system and put it in the pending queue
        auto* system = new System{nullptr, std::make_shared<SystemWrapper<F>>(func), {}};
        mPendingSystems.push_back(system);
        mCurrSystem = mPendingSystems.back();
    }

    template <typename F>
    void Dispatcher::systemSetAfterSystem(F func)
    {
        auto it = std::find_if(mPendingSystems.begin(), mPendingSystems.end(), [&func](const System* system) {
            auto* wrapper = dynamic_cast<SystemWrapper<F>*>(system->system.get());
            if (!wrapper)
            {
                return false;
            }
            return wrapper->mSystem == func;
        });
        if (it == mPendingSystems.end())
        {
            CUBOS_ERROR("Tried to set system after a non-existing system!");
            return;
        }
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS(mCurrSystem);
        System* system = *it;
        ENSURE_SYSTEM_SETTINGS(system);
        // Set curr to run after this system
        mCurrSystem->settings->after.system.push_back(system);
        // And this system to run before curr
        system->settings->before.system.push_back(mCurrSystem);
    }

    template <typename F>
    void Dispatcher::systemSetBeforeSystem(F func)
    {
        auto it = std::find_if(mPendingSystems.begin(), mPendingSystems.end(), [&func](const System* system) {
            auto* wrapper = dynamic_cast<SystemWrapper<F>*>(system->system.get());
            if (!wrapper)
            {
                return false;
            }
            return wrapper->mSystem == func;
        });
        if (it == mPendingSystems.end())
        {
            CUBOS_ERROR("Tried to set system before a non-existing system!");
            return;
        }
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS(mCurrSystem);
        System* system = *it;
        ENSURE_SYSTEM_SETTINGS(system);
        // Set curr to run before this system
        mCurrSystem->settings->before.system.push_back(system);
        // And this system to run after curr
        system->settings->after.system.push_back(mCurrSystem);
    }

    template <typename F>
    void Dispatcher::systemAddCondition(F func)
    {
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS(mCurrSystem);
        auto bit = assignConditionBit(func);
        mCurrSystem->settings->conditions |= bit;
    }

    template <typename F>
    std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS> Dispatcher::assignConditionBit(F func)
    {
        // Check if this condition already exists
        auto it = std::find_if(mConditions.begin(), mConditions.end(),
                               [&func](const std::shared_ptr<AnySystemWrapper<bool>>& condition) {
                                   auto wrapper = dynamic_cast<SystemWrapper<F>*>(condition.get());
                                   if (wrapper == nullptr)
                                   {
                                       return false;
                                   }
                                   return wrapper->mSystem == func;
                               });
        if (it != mConditions.end())
        {
            // If it does, return the set bit
            return std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>(1)
                   << static_cast<std::size_t>(it - mConditions.begin());
        }
        // If we already reached the condition limit, exit
        if (mConditions.size() >= CUBOS_CORE_DISPATCHER_MAX_CONDITIONS)
        {
            CUBOS_ERROR("The condition limit ({}) was reached!", CUBOS_CORE_DISPATCHER_MAX_CONDITIONS);
            abort();
        }
        // Otherwise, add it to the conditions list
        mConditions.push_back(std::make_shared<SystemWrapper<F>>(func));
        // And return the bit it has
        return std::bitset<CUBOS_CORE_DISPATCHER_MAX_CONDITIONS>(1) << (mConditions.size() - 1);
    }
} // namespace cubos::core::ecs
