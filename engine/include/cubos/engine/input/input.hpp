/// @file
/// @brief Resource @ref cubos::engine::Input.
/// @ingroup input-plugin

#pragma once

#include <cubos/core/io/window.hpp>

#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/event_tracker.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores the input bindings for multiple players.
    ///
    /// Its state is updated accordingly as events are received by the @ref input-plugin.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API Input final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Alias for @ref core::io::Key.
        using Key = core::io::Key;

        /// @brief Alias for @ref core::io::GamepadButton.
        using GamepadButton = core::io::GamepadButton;

        /// @brief Alias for @ref core::io::GamepadAxis.
        using GamepadAxis = core::io::GamepadAxis;

        /// @brief Alias for @ref core::io::MouseButton.
        using MouseButton = core::io::MouseButton;

        /// @brief Alias for @ref core::io::MouseButton.
        using MouseAxis = core::io::MouseAxis;

        Input() = default;
        ~Input() = default;

        /// @brief Handles a Window event.
        /// @param window Window that received the event.
        /// @param event Event to handle.
        void handle(const core::io::Window& window, const core::io::WindowEvent& event);

        /// @brief Updates the input state.
        /// @param window Window to get the input state from.
        void update(const core::io::Window& window);

        /// @brief Clears all bindings.
        void clear();

        /// @brief Clears all bindings for a specific player.
        /// @param player Player whose bindings will be cleared.
        void clear(int player);

        /// @brief Sets the bindings for a specific player.
        /// @param bindings Bindings to set.
        /// @param player Player whose bindings will be set.
        void bind(const InputBindings& bindings, int player = 0);

        /// @brief Binds an action to a specific player.
        /// @param actionName Name of the action.
        /// @param action Action to bind.
        /// @param player Player to whom the action will be bound.
        void bind(const char* actionName, const InputAction& action, int player = 0);

        /// @brief Binds an axis to a specific player.
        /// @param axisName Name of the axis.
        /// @param axis Axis to bind.
        /// @param player Player to whom the axis will be bound.
        void bind(const char* axisName, const InputAxis& axis, int player = 0);

        /// @brief Gets an action state for a specific player.
        /// @param actionName Name of the action.
        /// @param player Player whose action state will be retrieved.
        /// @return Action state.
        const InputAction& action(const char* actionName, int player = 0) const;

        /// @brief Gets an axis state for a specific player.
        /// @param axisName Name of the axis.
        /// @param player Player whose axis value will be retrieved.
        /// @return Axis state.
        const InputAxis& axis(const char* axisName, int player = 0) const;

        /// @brief Gets the bindings for each player.
        /// @return Bindings for each player.
        const std::unordered_map<int, InputBindings>& bindings() const;

    private:
        std::unordered_map<int, InputBindings> mBindings;
        // std::unordered_map<int, InputEventTracker> mEventTrackers;
        // std::unordered_map<int, bool> mNeedsUpdate;
    };
} // namespace cubos::engine
