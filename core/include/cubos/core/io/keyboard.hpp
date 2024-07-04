/// @file
/// @brief Enums @ref cubos::core::io::Key and @ref cubos::core::io::Modifiers.
/// @ingroup core-io

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::io
{
    /// @brief Keyboard key codes enum.
    /// @ingroup core-io
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

    /// @brief Keyboard modifier flags enum.
    /// @ingroup core-io
    enum class Modifiers
    {
        None = 0,
        Control = 1,
        Shift = 2,
        Alt = 4,
        System = 8,
    };

    inline Modifiers operator~(Modifiers mods)
    {
        return static_cast<Modifiers>(~static_cast<int>(mods));
    }

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
} // namespace cubos::core::io

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, cubos::core::io::Key);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, cubos::core::io::Modifiers);
