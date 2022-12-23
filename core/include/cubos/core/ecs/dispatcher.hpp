#ifndef CUBOS_CORE_ECS_DISPATCHER_HPP
#define CUBOS_CORE_ECS_DISPATCHER_HPP

#include <cubos/core/ecs/system.hpp>
#include <cubos/core/log.hpp>

#include <map>
#include <vector>
#include <string>
#include <memory>

#define ENSURE_CURR_SYSTEM()                                                                                           \
    if (!currSystem)                                                                                                   \
    {                                                                                                                  \
        CUBOS_ERROR("No system currently selected!");                                                                  \
        return;                                                                                                        \
    }
#define ENSURE_CURR_TAG()                                                                                              \
    if (!currTagSettings)                                                                                              \
    {                                                                                                                  \
        CUBOS_ERROR("No tag currently selected!");                                                                     \
        return;                                                                                                        \
    }
#define ENSURE_SYSTEM_SETTINGS(obj)                                                                                    \
    if (!obj->settings)                                                                                                \
    {                                                                                                                  \
        obj->settings = std::make_shared<SystemSettings>();                                                            \
    }

namespace cubos::core::ecs
{
    /// @brief Dispatcher is a class that is used to register systems and call them all at once, where they will be
    /// executed in order of the stages they are in.
    class Dispatcher
    {
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
            Dependency before, after;
            // TODO: Add run conditions, threading modes, etc...
            // TODO: Implement inherithance behavior
            std::vector<std::string> inherits;
        };

        /// Internal class to handle tag settings
        struct System
        {
            std::shared_ptr<SystemSettings> settings;
            std::shared_ptr<AnySystemWrapper<void>> system;
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
        };

        /// Visits a DFSNode to create a topological order.
        /// This is used internally during call chain compilation.
        /// @param node The node to visit.
        /// @param nodes Array of DFSNodes.
        /// @return True if a cycle was detected, false if otherwise.
        bool dfsVisit(DFSNode& node, std::vector<DFSNode>& nodes);

    public:
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

        /// Adds a system, and sets it as the current system for further
        /// settings.
        /// @param func System to add.
        template <typename F> void addSystem(F func);

        /// Sets the tag for the current system.
        /// @param tag The tag to run under.
        void systemSetTag(const std::string& tag);

        /// Sets the current system to run after the tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run after.
        void systemSetAfterTag(const std::string& tag);

        /// Sets the current system to run after a given system.
        /// The specified system must exist.
        /// @param func The system to run after.
        template <typename F> void systemSetAfterSystem(F func);

        /// Sets the current system to run before the tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run before.
        void systemSetBeforeTag(const std::string& tag);

        /// Sets the current system to run before the system.
        /// The specified system must exist.
        /// @param func The system to run before.
        template <typename F> void systemSetBeforeSystem(F func);

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
        /// Variables for holding information before call chain is compiled.
        std::vector<System> pendingSystems;                                 ///< All systems.
        std::map<std::string, std::shared_ptr<SystemSettings>> tagSettings; ///< All tags.
        System* currSystem;                                                 ///< Last set system, for changing settings.
        SystemSettings* currTagSettings;                                    ///< Last set tag, for changing settings.

        /// Variables for holding information after call chain is compiled.
        std::vector<System> systems; ///< Compiled order of running systems.
    };

    template <typename F> void Dispatcher::addSystem(F func)
    {
        // Wrap the system and put it in the pending queue
        System system = {nullptr, std::make_shared<SystemWrapper<F>>(func)};
        pendingSystems.push_back(std::move(system));
        currSystem = &pendingSystems.back();
    }

    template <typename F> void Dispatcher::systemSetAfterSystem(F func)
    {
        auto it = std::find_if(pendingSystems.begin(), pendingSystems.end(), [&func](const System& system) {
            SystemWrapper<F>* wrapper = dynamic_cast<SystemWrapper<F>*>(system.system.get());
            if (!wrapper)
                return false;
            return wrapper->system == func;
        });
        if (it == pendingSystems.end())
        {
            CUBOS_ERROR("Tried to set system after a non-existing system!");
            return;
        }
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS(currSystem);
        ENSURE_SYSTEM_SETTINGS(it);
        // Set curr to run after this system
        currSystem->settings->after.system.push_back(&(*it));
        // And this system to run before curr
        it->settings->before.system.push_back(currSystem);
    }

    template <typename F> void Dispatcher::systemSetBeforeSystem(F func)
    {
        auto it = std::find_if(pendingSystems.begin(), pendingSystems.end(), [&func](const System& system) {
            SystemWrapper<F>* wrapper = dynamic_cast<SystemWrapper<F>*>(system.system.get());
            if (!wrapper)
                return false;
            return wrapper->system == func;
        });
        if (it == pendingSystems.end())
        {
            CUBOS_ERROR("Tried to set system before a non-existing system!");
            return;
        }
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS(currSystem);
        ENSURE_SYSTEM_SETTINGS(it);
        // Set curr to run before this system
        currSystem->settings->before.system.push_back(&(*it));
        // And this system to run after curr
        it->settings->after.system.push_back(currSystem);
    }
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_DISPATCHER_HPP
