#ifndef CUBOS_ENGINE_CUBOS_HPP
#define CUBOS_ENGINE_CUBOS_HPP

#include <cubos/core/ecs/event_pipe.hpp>
#include <cubos/core/ecs/dispatcher.hpp>
#include <cubos/core/ecs/system.hpp>
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

    /// Resource used for storing program arguments.
    struct Arguments
    {
        const std::vector<std::string> value;
    };

    /// Used to chain configurations related to tags
    class TagBuilder
    {
    public:
        TagBuilder(core::ecs::Dispatcher& dispatcher);
        ~TagBuilder() = delete;

        /// Sets the current tag to be executed before another tag.
        /// @param tag The tag to be executed before.
        TagBuilder& beforeTag(const std::string& tag);

        /// Sets the current tag to be executed after another tag.
        /// @param tag The tag to be executed after.
        TagBuilder& afterTag(const std::string& tag);

    private:
        core::ecs::Dispatcher& dispatcher;
    };

    /// Used to chain configurations related to systems
    class SystemBuilder
    {
    public:
        SystemBuilder(core::ecs::Dispatcher& dispatcher);
        ~SystemBuilder() = delete;

        /// Sets the current system's tag.
        /// @param tag The tag to be set.
        SystemBuilder& tagged(const std::string& tag);

        /// Sets the current system to be executed before another tag.
        /// @param tag The tag to be executed before.
        SystemBuilder& beforeTag(const std::string& tag);

        /// Sets the current system to be executed after another tag.
        /// @param tag The tag to be executed after.
        SystemBuilder& afterTag(const std::string& tag);

        /// Sets the current system to be executed before another system.
        /// @param func The system to be executed before.
        template <typename F> SystemBuilder& beforeSystem(F func);

        /// Sets the current system to be executed after another system.
        /// @param func The system to be executed after.
        template <typename F> SystemBuilder& afterSystem(F func);

    private:
        core::ecs::Dispatcher& dispatcher;
    };

    /// Represents the engine itself, and exposes the interface with which the game developer interacts with.
    /// Ties up all the different parts of the engine together.
    class Cubos final
    {
    public:
        Cubos();
        Cubos(int argc, char** argv);
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

        /// Adds a new event type to the engine.
        /// @tparam E The type of the event.
        /// @return Reference to this object, for chaining.
        template <typename E> Cubos& addEvent();

        /// Sets the current tag for the main dispatcher. If the tag doesn't exist, it will be created.
        /// @param tag The tag to set.
        /// @return Tag builder used to configure the tag.
        TagBuilder& tag(const std::string& tag);

        /// Sets the current tag for the startup dispatcher. If the tag doesn't exist, it will be created.
        /// @param tag The tag to set.
        /// @return Tag builder used to configure the tag.
        TagBuilder& startupTag(const std::string& tag);

        /// Adds a new system to the engine, which will be executed at every iteration of the main loop.
        /// @tparam F The type of the system function.
        /// @param func The system function.
        /// @return System builder used to configure the system.
        template <typename F> SystemBuilder& system(F func);

        /// Adds a new startup system to the engine.
        /// Startup systems are executed only once, before the main loop starts.
        /// @tparam F The type of the system function.
        /// @param func The system function.
        /// @return System builder used to configure the system.
        template <typename F> SystemBuilder& startupSystem(F func);

        /// Runs the engine.
        void run();

    private:
        core::ecs::Dispatcher mainDispatcher, startupDispatcher;
        core::ecs::World world;
        std::set<void (*)(Cubos&)> plugins;
    };

    // Implementation.

    template <typename F> SystemBuilder& SystemBuilder::beforeSystem(F func)
    {
        dispatcher.systemSetBeforeSystem(func);
        return *this;
    }

    template <typename F> SystemBuilder& SystemBuilder::afterSystem(F func)
    {
        dispatcher.systemSetAfterSystem(func);
        return *this;
    }

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

    template <typename E> Cubos& Cubos::addEvent()
    {
        // The user could register this manually, but using this method is more convenient.
        world.registerResource<core::ecs::EventPipe<E>>();
        return *this;
    }

    template <typename F> SystemBuilder& Cubos::system(F func)
    {
        mainDispatcher.addSystem(func);
        SystemBuilder* builder = new SystemBuilder(mainDispatcher);

        return *builder;
    }

    template <typename F> SystemBuilder& Cubos::startupSystem(F func)
    {
        startupDispatcher.addSystem(func);
        SystemBuilder* builder = new SystemBuilder(startupDispatcher);

        return *builder;
    }
} // namespace cubos::engine

#endif // CUBOS_ENGINE_CUBOS_HPP
