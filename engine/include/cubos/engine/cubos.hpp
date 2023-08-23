/// @dir engine
/// @brief @ref cubos::engine namespace.

/// @file
/// @brief @ref Class cubos::engine::Cubos.

#pragma once

#include <set>
#include <string>

#include <cubos/core/ecs/dispatcher.hpp>
#include <cubos/core/ecs/event_pipe.hpp>
#include <cubos/core/ecs/system.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores the time since the last iteration of the main loop started.
    ///
    /// This resource is added and updated by the @ref Cubos class.
    struct DeltaTime
    {
        DeltaTime(float value);
        float value;
    };

    /// @brief Resource used as a flag to indicate whether the main loop should stop running.
    ///
    /// This resource is added by the @ref Cubos class, initially set to true.
    struct ShouldQuit
    {
        ShouldQuit(bool value);
        bool value;
    };

    /// @brief Resource used for storing program arguments.
    ///
    /// This resource is added by the @ref Cubos class when constructed with arguments.
    struct Arguments
    {
        Arguments(std::vector<std::string> value);
        const std::vector<std::string> value;
    };

    /// @brief Used to chain configurations related to tags.
    class TagBuilder
    {
    public:
        /// @brief Construct.
        /// @param dispatcher Dispatcher being configured
        /// @param tags Vector which stores the tags for this dispatcher.
        /// @return Reference to this object, for chaining.
        TagBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags);

        /// @brief Sets the current tag to be executed before another tag.
        /// @param tag Tag to be executed before.
        /// @return Reference to this object, for chaining.
        TagBuilder& beforeTag(const std::string& tag);

        /// @brief Sets the current tag to be executed after another tag.
        /// @param tag Tag to be executed after.
        /// @return Reference to this object, for chaining.
        TagBuilder& afterTag(const std::string& tag);

        /// @brief Adds a condition to the current tag. If this condition returns false, systems
        /// with this tag will not be executed. For the tagged systems to run, all conditions must
        /// return true.
        /// @param func Condition function.
        /// @return Reference to this object, for chaining.
        template <typename F>
        TagBuilder& runIf(F func);

    private:
        core::ecs::Dispatcher& mDispatcher;
        std::vector<std::string>& mTags;
    };

    /// @brief Used to chain configurations related to systems
    class SystemBuilder
    {
    public:
        /// @brief Construct.
        /// @param dispatcher Dispatcher being configured.
        /// @param tags Vector which stores the tags for this dispatcher.
        SystemBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags);

        /// @brief Sets the current system's tag.
        /// @param tag Tag to be set.
        /// @return Reference to this object, for chaining.
        SystemBuilder& tagged(const std::string& tag);

        /// @brief Sets the current system to be executed before another tag.
        /// @param tag Tag to be executed before.
        /// @return Reference to this object, for chaining.
        SystemBuilder& beforeTag(const std::string& tag);

        /// @brief Sets the current system to be executed after another tag.
        /// @param tag Tag to be executed after.
        /// @return Reference to this object, for chaining.
        SystemBuilder& afterTag(const std::string& tag);

        /// @brief Sets the current system to be executed before another system.
        /// @tparam F System type.
        /// @param func System to be executed before.
        /// @return Reference to this object, for chaining.
        template <typename F>
        SystemBuilder& beforeSystem(F func);

        /// @brief Sets the current system to be executed after another system.
        /// @param func System to be executed after.
        /// @tparam F System type.
        /// @return Reference to this object, for chaining.
        template <typename F>
        SystemBuilder& afterSystem(F func);

        /// @brief Adds a condition to the current system. If this condition returns false, the
        /// system will not be executed. For a system to run, all conditions must return true.
        /// @tparam F Condition system type.
        /// @param func Condition system.
        /// @return Reference to this object, for chaining.
        template <typename F>
        SystemBuilder& runIf(F func);

    private:
        core::ecs::Dispatcher& mDispatcher;
        std::vector<std::string>& mTags;
    };

    /// @brief Represents the engine itself, and exposes the interface with which the game
    /// developer interacts with. Ties up all the different parts of the engine together.
    class Cubos final
    {
    public:
        ~Cubos() = default;

        /// @brief Constructs an empty application without arguments.
        Cubos();

        /// @brief Constructs an empty application with arguments.
        /// @param argc Argument count.
        /// @param argv Argument array.
        Cubos(int argc, char** argv);

        /// @brief Adds a new plugin to the engine. If the plugin had already been added, nothing
        /// happens. A plugin is just a function that operates on the Cubos object, further
        /// configuring it. It is useful for separating the code into modules.
        /// @param plugin Plugin to add.
        /// @return Reference to this object, for chaining.
        Cubos& addPlugin(void (*func)(Cubos&));

        /// @brief Adds a new resource to the engine.
        /// @tparam R Type of the resource.
        /// @tparam TArgs Types of the arguments passed to the resource's constructor.
        /// @param args Arguments passed to the resource's constructor.
        /// @return Reference to this object, for chaining.
        template <typename R, typename... TArgs>
        Cubos& addResource(TArgs... args);

        /// @brief Adds a new component type to the engine.
        /// @tparam C Type of the component.
        /// @return Reference to this object, for chaining.
        template <typename C>
        Cubos& addComponent();

        /// @brief Adds a new event type to the engine.
        /// @tparam E Type of the event.
        /// @return Reference to this object, for chaining.
        template <typename E>
        Cubos& addEvent();

        /// @brief Returns a @ref TagBuilder to configure the given tag.
        /// @param tag Tag to configure.
        /// @return @ref TagBuilder used to configure the tag.
        TagBuilder tag(const std::string& tag);

        /// @brief Returns a @ref TagBuilder to configure the given startup tag.
        /// @param tag Tag to configure.
        /// @return @ref TagBuilder used to configure the tag.
        TagBuilder startupTag(const std::string& tag);

        /// @brief Adds a new system to the engine, which will be executed at every iteration of
        /// the main loop.
        /// @tparam F Type of the system function.
        /// @param func System function.
        /// @return @ref SystemBuilder used to configure the system.
        template <typename F>
        SystemBuilder system(F func);

        /// @brief Adds a new startup system to the engine, which will be executed only once,
        /// before the main loop starts.
        /// @tparam F Type of the system function.
        /// @param func System function.
        /// @return @ref SystemBuilder used to configure the system.
        template <typename F>
        SystemBuilder startupSystem(F func);

        /// @brief Runs the engine.
        ///
        /// Initially, dispatches all of the startup systems.
        /// Then, while @ref ShouldQuit is false, dispatches all other systems.
        void run();

    private:
        core::ecs::Dispatcher mMainDispatcher;
        core::ecs::Dispatcher mStartupDispatcher;
        core::ecs::World mWorld;
        std::set<void (*)(Cubos&)> mPlugins;
        std::vector<std::string> mMainTags;
        std::vector<std::string> mStartupTags;
    };

    // Implementation.

    template <typename F>
    TagBuilder& TagBuilder::runIf(F func)
    {
        mDispatcher.tagAddCondition(func);
        return *this;
    }

    template <typename F>
    SystemBuilder& SystemBuilder::beforeSystem(F func)
    {
        mDispatcher.systemSetBeforeSystem(func);
        return *this;
    }

    template <typename F>
    SystemBuilder& SystemBuilder::afterSystem(F func)
    {
        mDispatcher.systemSetAfterSystem(func);
        return *this;
    }

    template <typename F>
    SystemBuilder& SystemBuilder::runIf(F func)
    {
        mDispatcher.systemAddCondition(func);
        return *this;
    }

    template <typename R, typename... TArgs>
    Cubos& Cubos::addResource(TArgs... args)
    {
        mWorld.registerResource<R>(args...);
        return *this;
    }

    template <typename C>
    Cubos& Cubos::addComponent()
    {
        mWorld.registerComponent<C>();
        return *this;
    }

    template <typename E>
    Cubos& Cubos::addEvent()
    {
        // The user could register this manually, but using this method is more convenient.
        mWorld.registerResource<core::ecs::EventPipe<E>>();
        return *this;
    }

    template <typename F>
    SystemBuilder Cubos::system(F func)
    {
        mMainDispatcher.addSystem(func);
        return {mMainDispatcher, mStartupTags};
    }

    template <typename F>
    SystemBuilder Cubos::startupSystem(F func)
    {
        mStartupDispatcher.addSystem(func);
        return {mStartupDispatcher, mMainTags};
    }
} // namespace cubos::engine
