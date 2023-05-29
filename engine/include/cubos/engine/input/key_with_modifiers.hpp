#pragma once

#include <cubos/core/io/keyboard.hpp>

namespace cubos::engine
{
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

        const core::io::Key& key() const;
        const core::io::Modifiers& modifiers() const;

        core::io::Key& key();
        core::io::Modifiers& modifiers();

        bool pressed() const;
        void pressed(bool pressed);

    private:
        core::io::Key mKey = core::io::Key::Invalid;
        core::io::Modifiers mModifiers = core::io::Modifiers::None;

        /// Not serialized.
        bool mPressed = false;
    };
} // namespace cubos::engine