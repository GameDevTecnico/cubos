/// @file
/// @brief Contains the InputAction class.

#pragma once

#include <vector>

#include <cubos/core/io/keyboard.hpp>

namespace cubos::engine
{
    /// Used to represent a single input action, such as "jump" or "attack".
    /// It can be bound to multiple keys, and will be considered "pressed" if any of them are pressed.
    class InputAction final
    {
    public:
        InputAction() = default;
        InputAction(std::vector<std::pair<core::io::Key, core::io::Modifiers>> keys)
            : mKeys(keys)
        {
        }

        ~InputAction() = default;

        /// @return The vector of keys.
        const std::vector<std::pair<core::io::Key, core::io::Modifiers>>& keys() const;

        /// @return The vector of keys.
        std::vector<std::pair<core::io::Key, core::io::Modifiers>>& keys();

        /// @return Whether this action is pressed.
        bool pressed() const;

        /// Sets whether this action is pressed.
        /// @param pressed The new pressed state.
        void pressed(bool pressed);

    private:
        std::vector<std::pair<core::io::Key, core::io::Modifiers>> mKeys;

        /// Not serialized.
        bool mPressed;
    };
} // namespace cubos::engine
