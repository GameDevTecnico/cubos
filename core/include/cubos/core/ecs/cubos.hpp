/// @file
/// @brief Class @ref cubos::engine::Cubos.
/// @ingroup engine

#pragma once

#include <set>
#include <string>

#include <cubos/core/ecs/system/arguments/event/pipe.hpp>
#include <cubos/core/ecs/system/dispatcher.hpp>
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
        float value{0.0F};      ///< Time in seconds.
        float multiplier{1.0F}; ///< Multiplier which will be used when updating the value field.
    };

    /// @brief Resource used as a flag to indicate whether the main loop should stop running.
    ///
    /// This resource is added by the @ref Cubos class, initially set to true.
    ///
    /// @ingroup core-ecs
    struct ShouldQuit
    {
        bool value{true}; ///< Whether the main loop should stop running.
    };

    /// @brief Resource which stores the command-line arguments.
    ///
    /// This resource is added by the @ref Cubos class.
    ///
    /// @ingroup core-ecs
    struct Arguments
    {
        const std::vector<std::string> value; ///< Command-line arguments.
    };

    /// @brief Used to chain configurations related to tags.
    /// @ingroup core-ecs
    class TagBuilder
    {
    public:
        /// @brief Construct.
        /// @param world World.
        /// @param dispatcher Dispatcher being configured.
        /// @param tags Vector which stores the tags for this dispatcher.
        /// @return Reference to this object, for chaining.
        TagBuilder(World& world, core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags);

        /// @brief Sets the current tag to be executed before another tag.
        /// @param tag Tag to be executed before.
        /// @return Reference to this object, for chaining.
        TagBuilder& before(const std::string& tag);

        /// @brief Sets the current tag to be executed after another tag.
        /// @param tag Tag to be executed after.
        /// @return Reference to this object, for chaining.
        TagBuilder& after(const std::string& tag);

        /// @brief Tags all systems on this tag with the given tag.
        /// @param tag Tag to be inherited from.
        /// @return Reference to this object, for chaining.
        TagBuilder& tagged(const std::string& tag);

        /// @brief Adds a condition to the current tag. If this condition returns false, systems
        /// with this tag will not be executed. For the tagged systems to run, all conditions must
        /// return true.
        /// @param func Condition function.
        /// @return Reference to this object, for chaining.
        template <typename F>
        TagBuilder& runIf(F func);

        /// @brief Makes all systems within the tag repeat as a group until the given condition evaluates to false.
        /// @param func Condition function.
        /// @return Reference to this object, for chaining.
        template <typename F>
        TagBuilder& repeatWhile(F func);

    private:
        World& mWorld;
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
        core::ecs::World mWorld;
        core::ecs::Dispatcher mStartupDispatcher;
        core::ecs::Dispatcher mMainDispatcher;
        std::set<void (*)(Cubos&)> mPlugins;
        std::vector<std::string> mMainTags;
        std::vector<std::string> mStartupTags;
    };

    class Cubos::SystemBuilder
    {
    public:
        /// @brief Constructs.
        /// @param world World.
        /// @param dispatcher Dispatcher to add the system to.
        /// @param name Debug name.
        SystemBuilder(World& world, Dispatcher& dispatcher, std::string name);

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

        /// @brief Forces the next entity query argument to have the given target.
        /// @param target Target index. By default, the last specified target or 0.
        /// @return Builder.
        SystemBuilder&& entity(int target = -1) &&;

        /// @brief Forces the given target of the next query argument to have the following component.
        /// @param type Component type.
        /// @param target Target index. By default, the last specified target or 0.
        /// @return Builder.
        SystemBuilder&& with(const reflection::Type& type, int target = -1) &&;

        /// @copydoc with(const reflection::Type&, int)
        /// @tparam T Component type.
        template <reflection::Reflectable T>
        SystemBuilder&& with(int target = -1) &&
        {
            return std::move(*this).with(reflection::reflect<T>(), target);
        }

        /// @brief Forces the given target of the next query argument to not have the following component.
        /// @param type Component type.
        /// @param target Target. By default, the last specified target or 0.
        /// @return Builder.
        SystemBuilder&& without(const reflection::Type& type, int target = -1) &&;

        /// @copydoc without(const reflection::Type&, int)
        /// @tparam T Component type.
        template <reflection::Reflectable T>
        SystemBuilder&& without(int target = -1) &&
        {
            return std::move(*this).without(reflection::reflect<T>(), target);
        }

        /// @brief Forces the given targets of the next query argument to be related with the given relation.
        /// @param type Relation type.
        /// @param fromTarget From target index. By default, the last specified target or 0.
        /// @param toTarget From target index. By default, @p fromTarget + 1.
        /// @return Builder.
        SystemBuilder&& related(const reflection::Type& type, int fromTarget = -1, int toTarget = -1) &&;

        /// @copydoc related(const reflection::Type&, int, int)
        /// @tparam T Relation type.
        template <reflection::Reflectable T>
        SystemBuilder&& related(int fromTarget = -1, int toTarget = -1) &&
        {
            return std::move(*this).related(reflection::reflect<T>(), fromTarget, toTarget);
        }

        /// @brief Forces the given targets of the next query argument to be related with the given tree relation.
        /// @param type Relation type.
        /// @param traversal Tree traversal direction.
        /// @param fromTarget From target index. By default, the last specified target or 0.
        /// @param toTarget From target index. By default, @p fromTarget + 1.
        /// @return Builder.
        SystemBuilder&& related(const reflection::Type& type, Traversal traversal, int fromTarget = -1,
                                int toTarget = -1) &&;

        /// @copydoc related(const reflection::Type&, Traversal, int, int)
        /// @tparam T Relation type.
        template <reflection::Reflectable T>
        SystemBuilder&& related(Traversal traversal, int fromTarget = -1, int toTarget = -1) &&
        {
            return std::move(*this).related(reflection::reflect<T>(), traversal, fromTarget, toTarget);
        }

        /// @brief Makes the following argument options relative to the next argument.
        /// @return Builder.
        SystemBuilder&& other() &&;

        /// @brief Makes the system only run if the given condition evaluates to true.
        ///
        /// The condition runs immediately before the system runs, and is basically just like a normal system, but which
        /// returns a boolean instead of void.
        ///
        /// @param function Condition function.
        /// @return Builder.
        SystemBuilder&& onlyIf(auto function) &&
        {
            CUBOS_ASSERT(!mCondition.contains(), "Only one condition can be set per system");
            mCondition.replace(System<bool>::make(mWorld, std::move(function), {}));
            return std::move(*this);
        }

        /// @brief Finishes building the system with the given function.
        /// @param function System function.
        void call(auto function) &&
        {
            this->finish(System<void>::make(mWorld, std::move(function), mOptions));
        }

    private:
        /// @brief Finishes building the system with the given system body.
        /// @param system System wrapper.
        void finish(System<void> system);

        World& mWorld;
        Dispatcher& mDispatcher;
        std::string mName;
        Opt<System<bool>> mCondition;
        std::unordered_set<std::string> mTagged;
        std::unordered_set<std::string> mBefore;
        std::unordered_set<std::string> mAfter;
        std::vector<SystemOptions> mOptions;
        int mDefaultTarget{0};
    };

    // Implementation.

    template <typename F>
    TagBuilder& TagBuilder::runIf(F func)
    {
        mDispatcher.tagAddCondition(System<bool>::make(mWorld, std::move(func), {}));
        return *this;
    }

    template <typename F>
    TagBuilder& TagBuilder::repeatWhile(F func)
    {
        mDispatcher.tagRepeatWhile(System<bool>::make(mWorld, std::move(func), {}));
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
