/// @file
/// @brief Struct Gref cubos::core::io::GamepadState and related enums.
/// @ingroup core-io

#pragma once

#include <string>

namespace cubos::core::io
{
    /// @brief Gamepad buttons.
    /// @ingroup core-io
    enum class GamepadButton
    {
        Invalid = -1,

        A,
        B,
        X,
        Y,
        LBumper,
        RBumper,
        Back,
        Start,
        Guide,
        LThumb,
        RThumb,
        Up,
        Right,
        Down,
        Left,

        Count
    };

    /// @brief Gamepad axes.
    /// @ingroup core-io
    enum class GamepadAxis
    {
        Invalid = -1,

        LX,
        LY,
        RX,
        RY,
        LTrigger,
        RTrigger,

        Count
    };

    /// @brief Holds the state of a gamepad.
    /// @ingroup core-io
    struct GamepadState
    {
        /// @brief Which buttons are pressed, indexed by @ref GamepadButton.
        bool buttons[static_cast<int>(GamepadButton::Count)];

        /// @brief Values of the axes, indexed by @ref GamepadAxis.
        float axes[static_cast<int>(GamepadAxis::Count)];

        /// @brief Checks if a button is pressed.
        /// @param button Button to check.
        /// @return Whether it is pressed or not.
        bool pressed(GamepadButton button) const
        {
            return buttons[static_cast<int>(button)];
        }

        /// @brief Gets the value of an axis.
        /// @param axis Axis to get the value of.
        /// @return Value of the axis.
        float axis(GamepadAxis axis) const
        {
            return axes[static_cast<int>(axis)];
        }
    };

    /// @brief Converts a @ref GamepadButton enum to a string.
    /// @param button Button to convert.
    /// @return String representation.
    /// @ingroup core-io
    std::string gamepadButtonToString(GamepadButton button);

    /// @brief Converts a string to a @ref GamepadButton.
    /// @param str String to convert.
    /// @return Button.
    /// @ingroup core-io
    GamepadButton stringToGamepadButton(const std::string& str);

    /// @brief Convert a @ref GamepadAxis to a string.
    /// @param axis Axis to convert.
    /// @return String representation.
    /// @ingroup core-io
    std::string gamepadAxisToString(GamepadAxis axis);

    /// @brief Convert a string to a @ref GamepadAxis.
    /// @param str String to convert.
    /// @return Axis.
    /// @ingroup core-io
    GamepadAxis stringToGamepadAxis(const std::string& str);
} // namespace cubos::core::io
