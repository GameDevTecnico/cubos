/// @file
/// @brief Class @ref cubos::engine::InputAction.
/// @ingroup input-plugin

#pragma once

#include <vector>

#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/io/keyboard.hpp>

namespace cubos::engine
{
    /// @brief Stores the state of a single input action, such as "jump" or "attack".
    ///
    /// Can be bound to multiple keys, and will be considered "pressed" if any of them are pressed.
    ///
    /// @ingroup input-plugin
    class InputAction final
    {
    public:
        ~InputAction() = default;

        /// @brief Constructs without any bindings.
        InputAction() = default;

        /// @brief Constructs with existing bindings.
        /// @param keys Key bindings.
        /// @param gamepadButtons Gamepad button bindings.
        InputAction(std::vector<std::pair<core::io::Key, core::io::Modifiers>> keys,
                    std::vector<core::io::GamepadButton> gamepadButtons)
            : mKeys(keys)
            , mGamepadButtons(gamepadButtons)
        {
        }

        /// @brief Gets the key bindings.
        /// @return Vector of keys.
        const std::vector<std::pair<core::io::Key, core::io::Modifiers>>& keys() const;

        /// @brief Gets the key bindings.
        /// @return Vector of keys.
        std::vector<std::pair<core::io::Key, core::io::Modifiers>>& keys();

        /// @brief Gets the gamepad button bindings.
        /// @return Vector of buttons.
        const std::vector<core::io::GamepadButton>& gamepadButtons() const;

        /// @brief Gets the gamepad button bindings.
        /// @return Vector of buttons.
        std::vector<core::io::GamepadButton>& gamepadButtons();

        /// @brief Checks if this action is pressed.
        /// @return Whether this action is pressed.
        bool pressed() const;

        /// @brief Sets whether this action is pressed.
        /// @param pressed New pressed state.
        void pressed(bool pressed);

    private:
        std::vector<std::pair<core::io::Key, core::io::Modifiers>> mKeys;
        std::vector<core::io::GamepadButton> mGamepadButtons;

        bool mPressed; ///< Not serialized.
    };
} // namespace cubos::engine
