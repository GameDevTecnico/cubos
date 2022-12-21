#ifndef CUBOS_CORE_ECS_DISPATCHER_HPP
#define CUBOS_CORE_ECS_DISPATCHER_HPP

#include <cubos/core/ecs/system.hpp>
#include <cubos/core/log.hpp>

#include <map>
#include <vector>
#include <string>
#include <memory>

#define ENSURE_CURR_SYSTEM() if(!currSystem) { CUBOS_ERROR("No system currently selected!"); return; }
#define ENSURE_SYSTEM_SETTINGS() if(!currSystem->settings) { currSystem->settings = std::make_shared<SystemSettings>(); }

namespace cubos::core::ecs
{
    /// @brief Dispatcher is a class that is used to register systems and call them all at once, where they will be
    /// executed in order of the stages they are in.
    class Dispatcher
    {
    public:
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
            template <typename F> bool operator==(F other) const
            {
                SystemWrapper<F> wrapper = dynamic_cast<SystemWrapper<F>>(system.get());
                if(!wrapper) return false;
                return wrapper.system == other;
            }

            std::shared_ptr<SystemSettings> settings;
            std::unique_ptr<AnySystemWrapper> system;
        };

        struct DFSNode
        {
            enum { WHITE, GRAY, BLACK } m;
            System* s;
        };

        /// Adds a tag.
        /// @param tag Tag to add.
        void addTag(const std::string& tag);

        /// Makes a tag inherit the settings of another one.
        /// @param tag Tag to inherit.
        /// @param inherit Tag to inherit from.
        void inheritTag(const std::string& tag, const std::string& other);

        /// Adds a system. This system will be added to a pending queue
        /// and will properly be added when the call chain is compiled.
        /// @param system System to add.
        template <typename F> void addSystem(F func);

        /// Adds a tag for the current system.
        /// @param tag The tag to run under.
        void setTag(const std::string& tag);

        /// Sets the current system to run after the tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run after.
        void setAfter(const std::string& tag);

        /// Sets the current system to run after a given system.
        /// The specified system must exist.
        /// @param system The system to run after.
        template <typename F> void setAfter(F func);

        /// Sets the current system to run before the tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run before.
        void setBefore(const std::string& tag);

        /// Sets the current system to run before the system.
        /// The specified system must exist.
        /// @param system The system to run before.
        template <typename F> void setBefore(F func);

        /// Compiles a call chain. This takes all pending systems and
        /// determines their execution order.
        void compileChain();

        /// Calls all systems in order of the stages they are in.
        /// @param world World to call the systems in.
        void callSystems(World& world, Commands& cmds);

        /// Visits a DFSNode to create a topological order.
        /// This is used internally during call chain compilation.
        /// @param node The node to visit.
        /// @return True if a cycle was detected, false if otherwise.
        bool dfsVisit(DFSNode& node);

    private:
        /// Variables for holding information before call chain is compiled.
        std::vector<System> pendingSystems;               ///< All systems.
        System* currSystem;                                ///< Last set setting, for changing settings.
        std::map<std::string, std::shared_ptr<SystemSettings>> tagSettings; ///< All tags.

        /// Variables for holding information after call chain is compiled.
        std::vector<System> systems;
    };

    template <typename F> void Dispatcher::addSystem(F func)
    {
        // Wrap the system and put it in the pending queue
        System system = {nullptr, std::make_unique<SystemWrapper<F>>(func)};
        pendingSystems.push_back(std::move(system));
        currSystem = &pendingSystems.back();
    }

    template <typename F> void Dispatcher::setAfter(F func)
    {
        auto it = std::find(pendingSystems.begin(), pendingSystems.end(), func);
        if(it == pendingSystems.end())
        {
            CUBOS_ERROR("Tried to set system after a non-existing system!");
            return;
        }
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS();
        currSystem->settings->after.system.push_back(*it);
    }

    template <typename F> void Dispatcher::setBefore(F func)
    {
        auto it = std::find(pendingSystems.begin(), pendingSystems.end(), func);
        if(it == pendingSystems.end())
        {
            CUBOS_ERROR("Tried to set system before a non-existing system!");
            return;
        }
        ENSURE_CURR_SYSTEM();
        ENSURE_SYSTEM_SETTINGS();
        currSystem->settings->before.system.push_back(*it);
    }
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_DISPATCHER_HPP
