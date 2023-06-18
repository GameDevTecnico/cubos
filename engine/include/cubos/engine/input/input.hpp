/// @file
/// @brief Resource @ref cubos::engine::Input.
/// @ingroup input-plugin

#pragma once

#include <cubos/core/io/window.hpp>

#include <cubos/engine/input/bindings.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores the input bindings for multiple players.
    ///
    /// Its state is updated accordingly as events are received by the @ref input-plugin.
    ///
    /// @ingroup input-plugin
    class Input final
    {
    public:
        /// @brief Alias for @ref core::io::Key.
        using Key = core::io::Key;

        /// @brief Alias for @ref core::io::Modifiers.
        using Modifiers = core::io::Modifiers;

        Input() = default;
        ~Input() = default;

        /// @brief Clears all bindings.
        void clear();

        /// @brief Clears all bindings for a specific player.
        /// @param player Player whose bindings will be cleared.
        void clear(int player);

        /// @brief Sets the bindings for a specific player.
        /// @param bindings Bindings to set.
        /// @param player Player whose bindings will be set.
        void bind(const InputBindings& bindings, int player = 0);

        /// @brief Sets the gamepad for a specific player.
        /// @param player Player whose gamepad will be set.
        /// @param gamepad Gamepad to set.
        void gamepad(int player, int gamepad);

        /// @brief Gets the gamepad for a specific player.
        /// @param player Player whose gamepad will be retrieved.
        /// @return Gamepad if it exists, -1 otherwise.
        int gamepad(int player) const;

        /// @brief Gets an action state for a specific player.
        /// @param actionName Name of the action.
        /// @param player Player whose action state will be retrieved.
        /// @return Whether the action exists and is pressed.
        bool pressed(const char* actionName, int player = 0) const;

        /// @brief Gets an axis value for a specific player.
        /// @param axisName Name of the axis.
        /// @param player Player whose axis value will be retrieved.
        /// @return Axis value if the axis exists, 0.0 otherwise.
        float axis(const char* axisName, int player = 0) const;

        /// @brief Handle a key event.
        /// @param window Window that received the event.
        /// @param event Key event.
        void handle(const core::io::Window& window, const core::io::KeyEvent& event);

        /// @brief Handle all other events - discards them.
        ///
        /// This is method exists so that `std::visit` can be used with @ref core::io::WindowEvent
        /// on @ref handle().
        ///
        /// @param window Window that received the event.
        /// @param event Event to discard.
        inline void handle(const core::io::Window& window, const core::io::WindowEvent& event)
        {
            (void)window;
            (void)event;
        }

        /// @brief Gets the bindings for each player.
        /// @return Bindings for each player.
        const std::unordered_map<int, InputBindings>& bindings() const;

    private:
        struct BindingIndex
        {
            std::string name;      ///< Name of the action or axis.
            int player;            ///< Player index.
            bool negative = false; ///< Whether the pressed key is a negative axis key.
        };

        bool anyPressed(const core::io::Window& window, const std::vector<std::pair<Key, Modifiers>>& keys) const;
        void handleActions(const core::io::Window& window, const std::vector<BindingIndex>& boundActions);
        void handleAxes(const core::io::Window& window, const std::vector<BindingIndex>& boundAxes);

        std::unordered_map<int, InputBindings> mBindings;
        std::unordered_map<int, int> mGamepads;

        std::unordered_map<Key, std::vector<BindingIndex>> mBoundActions;
        std::unordered_map<Key, std::vector<BindingIndex>> mBoundAxes;
    };
} // namespace cubos::engine
