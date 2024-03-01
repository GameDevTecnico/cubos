/// @file
/// @brief Class @ref cubos::core::ecs::Cubos.
/// @ingroup core-ecs

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <cubos/core/ecs/system/arguments/event/pipe.hpp>
#include <cubos/core/ecs/system/planner.hpp>
#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/ecs/system/tag.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{

    /// @brief Resource which stores the time since the last iteration of the main loop started.
    ///
    /// This resource is added and updated by the @ref Cubos class.
    ///
    /// @ingroup core-ecs
    struct CUBOS_CORE_API DeltaTime
    {
        CUBOS_REFLECT;

        float unscaledValue{0.0F}; ///< Time in seconds, without any scaling.
        float scale{1.0F};         ///< Multiplier which will be used in @ref value().

        /// @brief Returns the time in seconds, affected by the @ref scale.
        /// @return Time in seconds.
        float value() const
        {
            return unscaledValue * scale;
        }
    };

    /// @brief Resource used as a flag to indicate whether the main loop should stop running.
    ///
    /// This resource is added by the @ref Cubos class, initially set to true.
    ///
    /// @ingroup core-ecs
    struct CUBOS_CORE_API ShouldQuit
    {
        CUBOS_REFLECT;

        bool value{true}; ///< Whether the main loop should stop running.
    };

    /// @brief Resource which stores the command-line arguments.
    ///
    /// This resource is added by the @ref Cubos class.
    ///
    /// @ingroup core-ecs
    struct CUBOS_CORE_API Arguments
    {
        CUBOS_REFLECT;

        std::vector<std::string> value; ///< Command-line arguments.
    };

    /// @brief Represents the engine itself, and exposes the interface with which the game
    /// developer interacts with. Ties up all the different parts of the engine together.
    /// @ingroup core-ecs
    class CUBOS_CORE_API Cubos final
    {
    public:
        /// @brief Function pointer type representing a plugin.
        using Plugin = void (*)(Cubos&);

        /// @brief Used to create new tags or configure existing ones.
        class TagBuilder;

        /// @brief Used to create new systems.
        class SystemBuilder;

        /// @brief Used to create new observers.
        class ObserverBuilder;

        ~Cubos() = default;

        /// @brief Constructs an empty application without arguments.
        Cubos();

        /// @brief Constructs an empty application with arguments.
        /// @param argc Argument count.
        /// @param argv Argument array.
        Cubos(int argc, char** argv);

        /// @brief Adds a new plugin as a sub-plugin of the current plugin.
        ///
        /// Aborts if the plugin has already been added by this or any other plugin.
        ///
        /// @param plugin Plugin.
        /// @return Cubos.
        Cubos& plugin(Plugin plugin);

        /// @brief Marks the given plugin as a dependency of the current plugin.
        ///
        /// Aborts if the plugin hasn't been added by this or any other plugin.
        ///
        /// @param plugin Plugin.
        /// @return Cubos.
        Cubos& depends(Plugin plugin);

        /// @brief Registers a new resource type, without initializing it.
        /// @param type Type.
        /// @return Cubos.
        Cubos& uninitResource(const reflection::Type& type);

        /// @brief Registers a new resource type with the given initial value.
        /// @param value Resource value.
        /// @return Cubos.
        Cubos& resource(memory::AnyValue value);

        /// @brief Registers a new resource type, without initializing it.
        /// @tparam T Type.
        /// @return Cubos.
        template <typename T>
        Cubos& uninitResource()
        {
            return this->uninitResource(reflection::reflect<T>());
        }

        /// @brief Registers a new resource type, constructing it with the given arguments.
        /// @tparam T Type.
        /// @tparam TArgs Constructor argument types.
        /// @param args Constructor arguments.
        /// @return Cubos.
        template <typename T, typename... TArgs>
        Cubos& resource(TArgs&&... args)
        {
            return this->resource(memory::AnyValue::customConstruct<T>(memory::forward<TArgs>(args)...));
        }

        /// @brief Registers a new component type.
        /// @param type Type.
        /// @return Cubos.
        Cubos& component(const reflection::Type& type);

        /// @brief Registers a new component type.
        /// @tparam T Type.
        /// @return Cubos.
        template <typename T>
        Cubos& component()
        {
            return this->component(reflection::reflect<T>());
        }

        /// @brief Registers a new relation type.
        /// @param type Type.
        /// @return Cubos.
        Cubos& relation(const reflection::Type& type);

        /// @brief Registers a new relation type.
        /// @tparam T Type.
        /// @return Cubos.
        template <typename T>
        Cubos& relation()
        {
            return this->relation(reflection::reflect<T>());
        }

        /// @brief Registers a new event type.
        /// @tparam T Type.
        /// @return Reference to this object, for chaining.
        template <typename T>
        Cubos& event()
        {
            // The user could register this manually, but this is more convenient.
            return this->resource<EventPipe<T>>();
        }

        /// @brief Registers a new tag. Returns a builder used to configure tagged systems.
        /// @param tag Tag.
        /// @return @ref TagBuilder.
        TagBuilder tag(const Tag& tag);

        /// @brief Registers a new startup tag. Returns a builder used to configure tagged systems.
        /// @param tag Tag.
        /// @return @ref TagBuilder.
        TagBuilder startupTag(const Tag& tag);

        /// @brief Returns a new builder used to add a system to the engine.
        /// @param name System debug name.
        /// @return Builder used to configure the system.
        SystemBuilder system(std::string name);

        /// @brief Returns a new builder used to add a startup system to the engine.
        /// @param name System debug name.
        /// @return Builder used to configure the system.
        SystemBuilder startupSystem(std::string name);

        /// @brief Returns a new builder used to add an observer to the engine.
        /// @param name Observer debug name.
        /// @return Builder used to configure the observer.
        ObserverBuilder observer(std::string name);

        /// @brief Runs the engine.
        ///
        /// Initially, dispatches all of the startup systems.
        /// Then, while @ref ShouldQuit is false, dispatches all other systems.
        void run();

    private:
        /// @brief Stores information regarding a plugin.
        struct PluginInfo
        {
            /// @brief How many plugins depend on this plugin.
            int dependentCount{0};

            /// @brief Plugins which this plugin depends on.
            std::unordered_set<Plugin> dependencies;

            /// @brief Plugins which were added by this plugin.
            std::unordered_set<Plugin> subPlugins;
        };

        /// @brief Stores information regarding a tag.
        struct TagInfo
        {
            /// @brief Whether the tag is a startup tag.
            bool isStartup;

            /// @brief Tag's identifier on its planner.
            Planner::TagId id;

            /// @brief Plugin which registered the tag.
            Plugin plugin;
        };

        /// @brief Checks if the given type was registered by the current plugin, its dependencies or sub-plugins.
        /// @param type Type.
        /// @return Whether the type was registered.
        bool isRegistered(const reflection::Type& type) const;

        /// @brief Checks if the given tag was registered by the current plugin, its dependencies or sub-plugins.
        /// @param tag Tag.
        /// @return Whether the tag was registered.
        bool isRegistered(const Tag& tag) const;

        /// @brief Checks if the given plugin is a sub-plugin of another.
        /// @param subPlugin Plugin to check if its a sub-plugin.
        /// @param basePlugin Plugin which may be the parent plugin.
        /// @return Whether @p subPlugin is really a sub-plugin of @p plugin.
        bool isSubPlugin(Plugin subPlugin, Plugin basePlugin) const;

        /// @brief Checks if the given plugin is equal, or included in some form by the given plugin.
        /// @param plugin Plugin which may be included.
        /// @param basePlugin Plugin which may include.
        bool isKnownPlugin(Plugin plugin, Plugin basePlugin) const;

        World mWorld;
        SystemRegistry mSystemRegistry;
        Planner mStartupPlanner;
        Planner mMainPlanner;

        /// @brief Stack with the plugins currently being configured.
        ///
        /// The null plugin represents the function which initializes the Cubos class and adds the root plugins.
        std::vector<Plugin> mPluginStack{nullptr};

        /// @brief Maps installed plugins to their information.
        std::unordered_map<Plugin, PluginInfo> mInstalledPlugins{{nullptr, {}}};

        /// @brief Maps registered types to the plugin they were registered in.
        memory::TypeMap<Plugin> mTypeToPlugin;

        /// @brief Maps registered tags to their data.
        std::unordered_map<std::string, TagInfo> mTags;
    };

    class CUBOS_CORE_API Cubos::TagBuilder
    {
    public:
        /// @brief Constructs.
        /// @param cubos Cubos.
        /// @param name Tag debug name.
        /// @param isStartup Whether the tag is a startup tag.
        /// @param tagId Tag identifier.
        TagBuilder(Cubos& cubos, std::string name, bool isStartup, Planner::TagId tagId);

        /// @brief Makes all tagged systems run before systems with the given tag.
        /// @param tag Tag.
        /// @return Builder.
        TagBuilder& before(const Tag& tag);

        /// @brief Makes all tagged systems run after systems with the given tag.
        /// @param tag Tag.
        /// @return Builder.
        TagBuilder& after(const Tag& tag);

        /// @brief Tags all tagged systems with the given tag.
        /// @param tag Tag.
        /// @return Builder.
        TagBuilder& tagged(const Tag& tag);

        /// @brief Equivalent to calling @ref tagged on the given tag.
        /// @param tag Tag.
        /// @return Builder.
        TagBuilder& addTo(const Tag& tag);

        /// @brief Makes all systems within the tag run only if the given condition evaluates to true.
        /// @param func Condition system function.
        /// @return Builder.
        TagBuilder& runIf(auto func)
        {
            auto conditionId = mCubos.mSystemRegistry.add(mName + "#condition",
                                                          System<bool>::make(mCubos.mWorld, std::move(func), {}));
            mPlanner.onlyIf(mTagId, conditionId);
            return *this;
        }

        /// @brief Makes all systems within the tag repeat while the given condition evaluates to true.
        /// @param func Condition system function.
        /// @return Builder.
        TagBuilder& repeatWhile(auto func)
        {
            auto conditionId =
                mCubos.mSystemRegistry.add(mName, System<bool>::make(mCubos.mWorld, std::move(func), {}));
            CUBOS_ASSERT(mPlanner.repeatWhile(mTagId, conditionId), "Tag was already set to repeating");
            return *this;
        }

    private:
        Cubos& mCubos;
        Planner& mPlanner;
        std::string mName;
        bool mIsStartup;
        Planner::TagId mTagId;
    };

    class CUBOS_CORE_API Cubos::SystemBuilder
    {
    public:
        /// @brief Constructs.
        /// @param cubos Cubos.
        /// @param isStartup Whether the system is a startup system.
        /// @param name Debug name.
        SystemBuilder(Cubos& cubos, bool isStartup, std::string name);

        /// @brief Adds a tag to the system.
        /// @param tag Tag.
        /// @return Builder.
        SystemBuilder&& tagged(const Tag& tag) &&;

        /// @brief Forces this system to only run after all systems with the given tag have finished.
        /// @param tag Tag.
        /// @return Builder.
        SystemBuilder&& before(const Tag& tag) &&;

        /// @brief Forces all systems with the given tag to run only after this system has finished.
        /// @param tag Tag.
        /// @return Builder.
        SystemBuilder&& after(const Tag& tag) &&;

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
        template <typename T>
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
        template <typename T>
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
        template <typename T>
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
        template <typename T>
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
            mCondition.replace(System<bool>::make(mCubos.mWorld, std::move(function), {}));
            return std::move(*this);
        }

        /// @brief Finishes building the system with the given function.
        /// @param function System function.
        void call(auto function) &&
        {
            this->finish(System<void>::make(mCubos.mWorld, std::move(function), mOptions));
        }

    private:
        /// @brief Finishes building the system with the given system body.
        /// @param system System wrapper.
        void finish(System<void> system);

        Cubos& mCubos;
        bool mIsStartup;
        std::string mName;
        Opt<System<bool>> mCondition;
        std::unordered_set<std::string> mTagged;
        std::unordered_set<std::string> mBefore;
        std::unordered_set<std::string> mAfter;
        std::vector<SystemOptions> mOptions;
        int mDefaultTarget{0};
    };

    class CUBOS_CORE_API Cubos::ObserverBuilder
    {
    public:
        /// @brief Constructs.
        /// @param cubos Cubos.
        /// @param name Debug name.
        ObserverBuilder(Cubos& cubos, std::string name);

        /// @brief Triggers the observer whenever the given component is added to an entity.
        /// @param type Component type.
        /// @param target Target index. By default, the last specified target or 0.
        /// @return Builder.
        ObserverBuilder&& onAdd(const reflection::Type& type, int target = -1) &&;

        /// @copydoc onAdd(const reflection::Type&, int)
        /// @tparam T Component type.
        template <typename T>
        ObserverBuilder&& onAdd(int target = -1) &&
        {
            return std::move(*this).onAdd(reflection::reflect<T>(), target);
        }

        /// @brief Triggers the observer whenever the given component is removed from an entity, or an entity with it is
        /// destroyed.
        /// @param type Component type.
        /// @param target Target index. By default, the last specified target or 0.
        /// @return Builder.
        ObserverBuilder&& onRemove(const reflection::Type& type, int target = -1) &&;

        /// @copydoc onRemove(const reflection::Type&, int)
        /// @tparam T Component type.
        template <typename T>
        ObserverBuilder&& onRemove(int target = -1) &&
        {
            return std::move(*this).onRemove(reflection::reflect<T>(), target);
        }

        /// @brief Forces the next entity query argument to have the given target.
        /// @param target Target index. By default, the last specified target or 0.
        /// @return Builder.
        ObserverBuilder&& entity(int target = -1) &&;

        /// @brief Forces the given target of the next query argument to have the following component.
        /// @param type Component type.
        /// @param target Target index. By default, the last specified target or 0.
        /// @return Builder.
        ObserverBuilder&& with(const reflection::Type& type, int target = -1) &&;

        /// @copydoc with(const reflection::Type&, int)
        /// @tparam T Component type.
        template <typename T>
        ObserverBuilder&& with(int target = -1) &&
        {
            return std::move(*this).with(reflection::reflect<T>(), target);
        }

        /// @brief Forces the given target of the next query argument to not have the following component.
        /// @param type Component type.
        /// @param target Target. By default, the last specified target or 0.
        /// @return Builder.
        ObserverBuilder&& without(const reflection::Type& type, int target = -1) &&;

        /// @copydoc without(const reflection::Type&, int)
        /// @tparam T Component type.
        template <typename T>
        ObserverBuilder&& without(int target = -1) &&
        {
            return std::move(*this).without(reflection::reflect<T>(), target);
        }

        /// @brief Forces the given targets of the next query argument to be related with the given relation.
        /// @param type Relation type.
        /// @param fromTarget From target index. By default, the last specified target or 0.
        /// @param toTarget From target index. By default, @p fromTarget + 1.
        /// @return Builder.
        ObserverBuilder&& related(const reflection::Type& type, int fromTarget = -1, int toTarget = -1) &&;

        /// @copydoc related(const reflection::Type&, int, int)
        /// @tparam T Relation type.
        template <typename T>
        ObserverBuilder&& related(int fromTarget = -1, int toTarget = -1) &&
        {
            return std::move(*this).related(reflection::reflect<T>(), fromTarget, toTarget);
        }

        /// @brief Forces the given targets of the next query argument to be related with the given tree relation.
        /// @param type Relation type.
        /// @param traversal Tree traversal direction.
        /// @param fromTarget From target index. By default, the last specified target or 0.
        /// @param toTarget From target index. By default, @p fromTarget + 1.
        /// @return Builder.
        ObserverBuilder&& related(const reflection::Type& type, Traversal traversal, int fromTarget = -1,
                                  int toTarget = -1) &&;

        /// @copydoc related(const reflection::Type&, Traversal, int, int)
        /// @tparam T Relation type.
        template <typename T>
        ObserverBuilder&& related(Traversal traversal, int fromTarget = -1, int toTarget = -1) &&
        {
            return std::move(*this).related(reflection::reflect<T>(), traversal, fromTarget, toTarget);
        }

        /// @brief Makes the following argument options relative to the next argument.
        /// @return Builder.
        ObserverBuilder&& other() &&;

        /// @brief Finishes building the observer with the given function.
        /// @param function System function.
        void call(auto function) &&
        {
            this->finish(System<void>::make(mCubos.mWorld, std::move(function), mOptions));
        }

    private:
        /// @brief Finishes building the system with the given system body.
        /// @param system System wrapper.
        void finish(System<void> system);

        Cubos& mCubos;
        std::string mName;
        std::vector<SystemOptions> mOptions;
        int mDefaultTarget{0};
        bool mRemove{false};
        ColumnId mColumnId{ColumnId::Invalid};
    };
} // namespace cubos::core::ecs
