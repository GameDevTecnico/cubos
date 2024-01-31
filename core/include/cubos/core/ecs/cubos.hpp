/// @file
/// @brief Class @ref cubos::engine::Cubos.
/// @ingroup engine

#pragma once

#include <set>
#include <string>

#include <cubos/core/ecs/system/dispatcher.hpp>
#include <cubos/core/ecs/system/event/pipe.hpp>
#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// @brief Resource which stores the time since the last iteration of the main loop started.
    ///
    /// This resource is added and updated by the @ref Cubos class.
    ///
    /// @ingroup core-ecs
    struct DeltaTime
    {
        DeltaTime(float value);

        float value; ///< Time in seconds.
    };

    /// @brief Resource used as a flag to indicate whether the main loop should stop running.
    ///
    /// This resource is added by the @ref Cubos class, initially set to true.
    ///
    /// @ingroup core-ecs
    struct ShouldQuit
    {
        ShouldQuit(bool value);

        bool value; ///< Whether the main loop should stop running.
    };

    /// @brief Resource which stores the command-line arguments.
    ///
    /// This resource is added by the @ref Cubos class.
    ///
    /// @ingroup core-ecs
    struct Arguments
    {
        Arguments(std::vector<std::string> value);

        const std::vector<std::string> value; ///< Command-line arguments.
    };

    /// @brief Used to chain configurations related to tags.
    /// @ingroup core-ecs
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
        TagBuilder& before(const std::string& tag);

        /// @brief Sets the current tag to be executed after another tag.
        /// @param tag Tag to be executed after.
        /// @return Reference to this object, for chaining.
        TagBuilder& after(const std::string& tag);

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

    /// @brief Represents the engine itself, and exposes the interface with which the game
    /// developer interacts with. Ties up all the different parts of the engine together.
    /// @ingroup core-ecs
    class Cubos final
    {
    public:
        /// @brief Used to create new systems.
        class SystemBuilder;

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
        /// @param func Entry point of the plugin to add.
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

        /// @brief Adds a new relation type to the engine.
        /// @tparam C Type of the relation.
        /// @return Reference to this object, for chaining.
        template <typename R>
        Cubos& addRelation()
        {
            mWorld.registerRelation<R>();
            return *this;
        }

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

        /// @brief Returns a new builder used to add a system to the engine.
        /// @param name System debug name.
        /// @return Builder used to configure the system.
        SystemBuilder system(std::string name);

        /// @brief Returns a new builder used to add a startup system to the engine.
        /// @param name System debug name.
        /// @return Builder used to configure the system.
        SystemBuilder startupSystem(std::string name);

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

    class Cubos::SystemBuilder
    {
    public:
        /// @brief Constructs.
        /// @param dispatcher Dispatcher to add the system to.
        /// @param name Debug name.
        SystemBuilder(Dispatcher& dispatcher, std::string name);

        /// @brief Adds a tag to the system.
        /// @param tag Tag.
        /// @return Builder.
        SystemBuilder&& tagged(const std::string& tag) &&;

        /// @brief Forces this system to only run after all systems with the given tag have finished.
        /// @param tag Tag.
        /// @return Builder.
        SystemBuilder&& before(const std::string& tag) &&;

        /// @brief Forces all systems with the given tag to run only after this system has finished.
        /// @param tag Tag.
        /// @return Builder.
        SystemBuilder&& after(const std::string& tag) &&;

        /// @brief Makes the system only run if the given condition evaluates to true.
        ///
        /// The condition runs immediately before the system runs, and is basically just like a normal system, but which
        /// returns a boolean instead of void.
        ///
        /// @param function Condition function.
        /// @return Builder.
        SystemBuilder&& onlyIf(auto function) &&
        {
            mCondition = std::make_shared<SystemWrapper<decltype(function)>>(function);
            return std::move(*this);
        }

        /// @brief Finishes building the system with the given function.
        /// @param function System function.
        void call(auto function) &&
        {
            this->finish(std::make_shared<SystemWrapper<decltype(function)>>(function));
        }

    private:
        /// @brief Finishes building the system with the given system body.
        /// @param system System wrapper.
        void finish(std::shared_ptr<AnySystemWrapper<void>> system);

        Dispatcher& mDispatcher;
        std::string mName;
        std::shared_ptr<AnySystemWrapper<bool>> mCondition{nullptr};
        std::unordered_set<std::string> mTagged;
        std::unordered_set<std::string> mBefore;
        std::unordered_set<std::string> mAfter;
    };

    // Implementation.

    template <typename F>
    TagBuilder& TagBuilder::runIf(F func)
    {
        mDispatcher.tagAddCondition(std::make_shared<SystemWrapper<decltype(func)>>(func));
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
} // namespace cubos::core::ecs
