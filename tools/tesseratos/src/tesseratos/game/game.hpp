/// @file
/// @brief Resource @ref tesseratos::Game.
/// @ingroup tesseratos-game-plugin

#pragma once

#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/memory/function.hpp>

namespace tesseratos
{
    /// @brief Resource which holds the game's Cubos instance.
    ///
    /// The game may be in four different states:
    /// - @b Unloaded - Game plugin has not been loaded yet.
    /// - @b Loaded - Game plugin has been loaded.
    /// - @b Running - Game has been started, and is currently running.
    /// - @b Paused - Game has been started, but is currently paused.
    ///
    /// @ingroup tesseratos-game-plugin
    class Game
    {
    public:
        CUBOS_REFLECT;

        /// @brief Marks the given target plugin so that when the game starts, it is replaced by the given plugin.
        /// @param target Target plugin.
        /// @param plugin Plugin to be injected.
        void inject(cubos::core::ecs::Plugin target, cubos::core::ecs::Plugin plugin);

        /// @brief Adds a resource to the game which will be injected into the game's world before startup.
        /// @param resource Resource.
        void resource(cubos::core::memory::AnyValue resource);

        /// @brief Adds a resource to the game which will be injected into the game's world before startup.
        /// @tparam T Resource type.
        /// @param resource Resource.
        template <typename T, typename... TArgs>
        void resource(TArgs&&... args)
        {
            this->resource(cubos::core::memory::AnyValue::customConstruct<T, TArgs...>(
                cubos::core::memory::forward<TArgs>(args)...));
        }

        /// @brief Unloads the game.
        ///
        /// Does nothing if the game has not been loaded.
        void unload();

        /// @brief Loads the game from the given plugin.
        ///
        /// If the game has already been loaded, it is unloaded first.
        ///
        /// @param plugin Game plugin.
        void load(cubos::core::ecs::Plugin plugin);

        /// @brief If the game has been started, resets it to the loaded state.
        ///
        /// Equivalent to calling @ref load again with the currently loaded plugin.
        void reset();

        /// @brief Runs the game's startup systems.
        ///
        /// Aborts if the game if it has not been loaded, or if it has already been started.
        void start();

        /// @brief Runs the game's update systems.
        ///
        /// If the game is paused, does nothing.
        /// Aborts if the game has not been started.
        void update();

        /// @brief Pauses the game.
        ///
        /// Aborts if the game has not been started.
        /// Does nothing if the game is already paused.
        void pause();

        /// @brief Resumes the game.
        ///
        /// Aborts if the game has not been started.
        /// Does nothing if the game is not paused.
        void resume();

        /// @brief Returns whether the game has been loaded.
        /// @return Whether the game has been loaded.
        bool isLoaded() const;

        /// @brief Returns whether the game has been started.
        /// @return Whether the game has been started.
        bool isStarted() const;

        /// @brief Returns whether the game is running.
        /// @return Whether the game is running.
        bool isRunning() const;

        /// @brief Returns the game's world.
        ///
        /// Aborts if the game has not been loaded.
        cubos::core::ecs::World& world();

        /// @brief Adds a context switch function.
        ///
        /// The function will be called before executing any game code, and again after.
        /// It receives a boolean which is true if we're entering the game code, and false if we're leaving.
        ///
        /// @param func Context switch function.
        void addContextSwitch(cubos::core::memory::Function<void(bool)> func);

    private:
        /// @brief Whether the game has been paused.
        bool mPaused{false};

        /// @brief Game's Cubos instance.
        cubos::core::ecs::Cubos mCubos{};

        /// @brief Loaded game plugin.
        cubos::core::ecs::Plugin mPlugin{nullptr};

        /// @brief Plugins to be injected.
        std::unordered_map<cubos::core::ecs::Plugin, cubos::core::ecs::Plugin> mInjections{};

        /// @brief Resources to be added.
        std::vector<cubos::core::memory::AnyValue> mResources{};

        /// @brief Context switch functions.
        std::vector<cubos::core::memory::Function<void(bool)>> mContextSwitches{};
    };
} // namespace tesseratos
