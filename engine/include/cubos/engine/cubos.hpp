#ifndef CUBOS_ENGINE_CUBOS_HPP
#define CUBOS_ENGINE_CUBOS_HPP

#include <cubos/core/ecs/dispatcher.hpp>
#include <cubos/core/ecs/world.hpp>

#include <set>
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

        /// Adds a new system to the engine, which will be executed every iteration of the main loop.
        /// If the stage doesn't already exist, it is created in the previously specified default position.
        /// @tparam F The type of the system function.
        /// @param func The system function.
        /// @param stage The stage in which the system should be executed.
        template <typename F> Cubos& addSystem(F func, std::string stage);

        /// Adds a new startup system to the engine.
        /// Startup systems are executed before the main loop starts.
        /// If the stage doesn't already exist, it is created in the previously specified default position.
        /// @tparam F The type of the system function.
        /// @param func The system function.
        /// @param stage The stage in which the system should be executed.
        template <typename F> Cubos& addStartupSystem(F func, std::string stage);

        /// Runs the engine.
        void run();

    private:
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

    template <typename F> Cubos& Cubos::addSystem(F func, std::string stage)
    {
        mainDispatcher.addSystem(func, stage);
        return *this;
    }

    template <typename F> Cubos& Cubos::addStartupSystem(F func, std::string stage)
    {
        startupDispatcher.addSystem(func, stage);
        return *this;
    }
} // namespace cubos::engine

#endif // CUBOS_ENGINE_CUBOS_HPP
