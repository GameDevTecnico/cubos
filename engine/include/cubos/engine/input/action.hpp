/// @file
/// @brief Contains the InputAction class.
#pragma once

#include <vector>

#include <cubos/engine/input/key_with_modifiers.hpp>

namespace cubos::engine
{
    /// Used to represent a single input action, such as "jump" or "attack".
    /// It can be bound to multiple keys, and will be considered "pressed" if any of them are pressed.
    class InputAction final
    {
    public:
        InputAction() = default;
        InputAction(std::vector<KeyWithModifiers> keys)
            : mKeys(keys)
        {
        }

        ~InputAction() = default;

        /// Gets the keys that will trigger this action.
        /// @return The vector of keys.
        const std::vector<KeyWithModifiers>& keys() const;

        /// Gets the keys that will trigger this action.
        /// @return The vector of keys.
        std::vector<KeyWithModifiers>& keys();

        /// Gets whether this action is pressed.
        /// @return Whether this action is pressed.
        bool pressed() const;

        /// Sets whether this action is pressed.
        /// @param pressed The new pressed state.
        void pressed(bool pressed);

    private:
        std::vector<KeyWithModifiers> mKeys;

        /// Not serialized.
        bool mPressed;
    };
} // namespace cubos::engine
