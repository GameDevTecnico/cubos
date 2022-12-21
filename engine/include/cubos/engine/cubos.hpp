#ifndef CUBOS_ENGINE_CUBOS_HPP
#define CUBOS_ENGINE_CUBOS_HPP

#include <cubos/core/ecs/dispatcher.hpp>
#include <cubos/core/ecs/system.hpp>
#include <cubos/core/ecs/world.hpp>

#include <set>
#include <map>
#include <string>

namespace cubos::engine
{
    /// Resource used as a flag to indicate whether the engine should stop running.
    struct ShouldQuit
    {
        bool value;
    };

    /// Represents the engine itself, and exposes the interface with which the game developer interacts with.
    /// Ties up all the different parts of the engine together.
    class Cubos final
    {
    public:
        Cubos();
        ~Cubos() = default;

        /// Adds a new plugin to the engine. If the plugin had already been added, nothing happens.
        /// A plugin is just a function that operates on the Cubos object, further configuring it.
        /// It is useful for separating the code into modules.
        /// @param plugin The plugin to add.
        /// @return Reference to this object, for chaining.
        Cubos& addPlugin(void (*func)(Cubos&));

        /// Adds a new resource to the engine.
        /// @tparam R The type of the resource.
        /// @tparam TArgs The types of the arguments passed to the resource's constructor.
        /// @param args The arguments passed to the resource's constructor.
        /// @return Reference to this object, for chaining.
        template <typename R, typename... TArgs> Cubos& addResource(TArgs... args);

        /// Adds a new component type to the engine.
        /// @tparam C The type of the component.
        /// @return Reference to this object, for chaining.
        template <typename C> Cubos& addComponent();

        /// Sets the current tag for the main dispatcher. If the tag doesn't exist, it will be created.
        /// Subsequent calls will set this tag's settings.
        /// @param tag The tag to set.
        Cubos& tag(const std::string& tag);

        /// Sets the current tag for the startup dispatcher. If the tag doesn't exist, it will be created.
        /// Subsequent calls will set this tag's settings.
        /// @param tag The tag to set.
        Cubos& startupTag(const std::string& tag);

        /// Adds a new system to the engine, which will be executed at every iteration of the main loop.
        /// @tparam F The type of the system function.
        /// @param func The system function.
        template <typename F> Cubos& system(F func);

        /// Adds a new startup system to the engine.
        /// Startup systems are executed only once, before the main loop starts.
        /// @tparam F The type of the system function.
        /// @param func The system function.
        template <typename F> Cubos& startupSystem(F func);

        /// Defines the tag for the current `system`/`startupSystem` set.
        /// @param tag The tag to run under.
        Cubos& tagged(const std::string& tag);

        /// Sets the current system to run after the tag.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run after.
        Cubos& after(const std::string& tag);

        /// Sets a given stage to happen before another stage.
        /// If the specified tag doesn't exist, it is internally created.
        /// @param tag The tag to run after.
        Cubos& before(const std::string& tag);

        /// Runs the engine.
        void run();

    private:
        bool isStartup;

        core::ecs::Dispatcher mainDispatcher, startupDispatcher;
        core::ecs::World world;
        std::set<void (*)(Cubos&)> plugins;
    };

    // Implementation.

    template <typename R, typename... TArgs> Cubos& Cubos::addResource(TArgs... args)
    {
        world.registerResource<R>(args...);
        return *this;
    }

    template <typename C> Cubos& Cubos::addComponent()
    {
        world.registerComponent<C>();
        return *this;
    }

    template <typename F> Cubos& Cubos::system(F func)
    {
        mainDispatcher.addSystem<F>(func);
        isStartup = false;

        return *this;
    }

    template <typename F> Cubos& Cubos::startupSystem(F func)
    {
        startupDispatcher.addSystem<F>(func);
        isStartup = true;

        return *this;
    }
} // namespace cubos::engine

#endif // CUBOS_ENGINE_CUBOS_HPP
