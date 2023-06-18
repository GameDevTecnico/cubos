/// @file
/// @brief Struct Gref cubos::core::io::GamepadState and related enums.
/// @ingroup core-io

#pragma once

namespace cubos::core::io
{
    /// @brief Gamepad buttons.
    /// @ingroup core-io
    enum class GamepadButton
    {
        A = 0,
        B,
        X,
        Y,
        LeftBumper,
        RightBumper,
        Back,
        Start,
        Guide,
        LeftThumb,
        RightThumb,
        DPadUp,
        DPadRight,
        DPadDown,
        DPadLeft,

        Count
    };

    /// @brief Gamepad axes.
    /// @ingroup core-io
    enum class GamepadAxis
    {
        LX = 0,
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
        bool pressed(GamepadButton button)
        {
            return buttons[static_cast<int>(button)];
        }

        /// @brief Gets the value of an axis.
        /// @param axis Axis to get the value of.
        /// @return Value of the axis.
        float axis(GamepadAxis axis)
        {
            return axes[static_cast<int>(axis)];
        }
    };
} // namespace cubos::core::io
