#pragma once

#include <string>

namespace cubos::core::io
{
    /// Keyboard key codes enum.
    enum class Key
    {
        Invalid = -1,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        Escape,
        LControl,
        LShift,
        LAlt,
        LSystem,
        RControl,
        RShift,
        RAlt,
        RSystem,
        Menu,
        LBracket,
        RBracket,
        SemiColon,
        Comma,
        Period,
        Quote,
        Slash,
        BackSlash,
        Tilde,
        Equal,
        Dash,
        Space,
        Return,
        BackSpace,
        Tab,
        PageUp,
        PageDown,
        End,
        Home,
        Insert,
        Delete,
        Add,
        Subtract,
        Multiply,
        Divide,
        Left,
        Right,
        Up,
        Down,
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        Pause,

        Count
    };

    /// Keyboard modifier flags enum.
    enum class Modifiers
    {
        None = 0,
        Control = 1,
        Shift = 2,
        Alt = 4,
        System = 8,
    };

    // Operator overloads for Modifiers.

    inline Modifiers operator|(Modifiers lhs, Modifiers rhs)
    {
        return static_cast<Modifiers>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline Modifiers operator&(Modifiers lhs, Modifiers rhs)
    {
        return static_cast<Modifiers>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    inline Modifiers& operator|=(Modifiers& lhs, Modifiers rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline Modifiers& operator&=(Modifiers& lhs, Modifiers rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    /// Convert a key to a string.
    inline std::string keyToString(Key key)
    {
        char key_char = static_cast<char>(key);

        if (key_char >= 0 && key_char <= 25)
        {
            return std::string(1, key_char + 'a');
        }

        // TODO: Add more keys.
        switch (key)
        {
        case Key::Up:
            return "Up";
        case Key::Down:
            return "Down";
        case Key::Left:
            return "Left";
        case Key::Right:
            return "Right";
        default:
            return "Invalid";
        }
    }

    /// Convert a string to a key.
    inline Key stringToKey(const std::string& str)
    {
        if (str.size() == 1)
        {
            char key_char = str[0];

            if (key_char >= 'a' && key_char <= 'z')
            {
                return static_cast<Key>(key_char - 'a');
            }
        }

        // TODO: Add more keys.
        if (str == "Up")
            return Key::Up;
        else if (str == "Down")
            return Key::Down;
        else if (str == "Left")
            return Key::Left;
        else if (str == "Right")
            return Key::Right;
        else
            return Key::Invalid;
    }

} // namespace cubos::core::io
