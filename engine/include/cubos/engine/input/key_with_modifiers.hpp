/// @file
/// @brief Contains the KeyWithModifiers class.
#pragma once

#include <cubos/core/io/keyboard.hpp>

namespace cubos::engine
{
    /// Used to represent a single key with modifiers, such as "Ctrl + A".
    class KeyWithModifiers final
    {
    public:
        KeyWithModifiers() = default;
        KeyWithModifiers(core::io::Key key, core::io::Modifiers modifiers)
            : mKey(key)
            , mModifiers(modifiers)
        {
        }

        ~KeyWithModifiers() = default;

        /// @return The key.
        const core::io::Key& key() const;

        /// @return The modifiers.
        const core::io::Modifiers& modifiers() const;

        /// @return The key.
        core::io::Key& key();

        /// @return The modifiers.
        core::io::Modifiers& modifiers();

        /// @return Whether the key with modifiers is pressed.
        bool pressed() const;

        /// Sets whether the key with modifiers is pressed.
        /// @param pressed The new pressed state.
        void pressed(bool pressed);

    private:
        core::io::Key mKey = core::io::Key::Invalid;
        core::io::Modifiers mModifiers = core::io::Modifiers::None;

        /// Not serialized.
        bool mPressed = false;
    };
} // namespace cubos::engine
