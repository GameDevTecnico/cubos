#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/io/keyboard.hpp>

using cubos::core::data::Deserializer;
using cubos::core::data::Serializer;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;
using namespace cubos::core;

std::string io::modifiersToString(Modifiers modifiers)
{
    std::string result;

    if ((modifiers & Modifiers::Control) != Modifiers::None)
    {
        result += "C-";
    }

    if ((modifiers & Modifiers::Shift) != Modifiers::None)
    {
        result += "S-";
    }

    if ((modifiers & Modifiers::Alt) != Modifiers::None)
    {
        result += "M-";
    }

    if ((modifiers & Modifiers::System) != Modifiers::None)
    {
        result += "D-";
    }

    return result;
}

template <>
void data::serialize<Modifiers>(Serializer& ser, const Modifiers& obj, const char* name)
{
    ser.write(io::modifiersToString(obj), name);
}

Modifiers io::stringToModifiers(const std::string& str)
{
    Modifiers result = Modifiers::None;

    if (str.find("C-") != std::string::npos)
    {
        result |= Modifiers::Control;
    }

    if (str.find("S-") != std::string::npos)
    {
        result |= Modifiers::Shift;
    }

    if (str.find("M-") != std::string::npos)
    {
        result |= Modifiers::Alt;
    }

    if (str.find("D-") != std::string::npos)
    {
        result |= Modifiers::System;
    }

    return result;
}

template <>
void data::deserialize<Modifiers>(Deserializer& des, Modifiers& obj)
{
    std::string str;
    des.read(str);
    obj = io::stringToModifiers(str);
}

std::string io::keyToString(Key key)
{
#define MAP_KEY_TO_STRING(key, string)                                                                                 \
    case Key::key:                                                                                                     \
        return (string);
    switch (key)
    {
        MAP_KEY_TO_STRING(A, "a")
        MAP_KEY_TO_STRING(B, "b")
        MAP_KEY_TO_STRING(C, "c")
        MAP_KEY_TO_STRING(D, "d")
        MAP_KEY_TO_STRING(E, "e")
        MAP_KEY_TO_STRING(F, "f")
        MAP_KEY_TO_STRING(G, "g")
        MAP_KEY_TO_STRING(H, "h")
        MAP_KEY_TO_STRING(I, "i")
        MAP_KEY_TO_STRING(J, "j")
        MAP_KEY_TO_STRING(K, "k")
        MAP_KEY_TO_STRING(L, "l")
        MAP_KEY_TO_STRING(M, "m")
        MAP_KEY_TO_STRING(N, "n")
        MAP_KEY_TO_STRING(O, "o")
        MAP_KEY_TO_STRING(P, "p")
        MAP_KEY_TO_STRING(Q, "q")
        MAP_KEY_TO_STRING(R, "r")
        MAP_KEY_TO_STRING(S, "s")
        MAP_KEY_TO_STRING(T, "t")
        MAP_KEY_TO_STRING(U, "u")
        MAP_KEY_TO_STRING(V, "v")
        MAP_KEY_TO_STRING(W, "w")
        MAP_KEY_TO_STRING(X, "x")
        MAP_KEY_TO_STRING(Y, "y")
        MAP_KEY_TO_STRING(Z, "z")
        MAP_KEY_TO_STRING(Num0, "0")
        MAP_KEY_TO_STRING(Num1, "1")
        MAP_KEY_TO_STRING(Num2, "2")
        MAP_KEY_TO_STRING(Num3, "3")
        MAP_KEY_TO_STRING(Num4, "4")
        MAP_KEY_TO_STRING(Num5, "5")
        MAP_KEY_TO_STRING(Num6, "6")
        MAP_KEY_TO_STRING(Num7, "7")
        MAP_KEY_TO_STRING(Num8, "8")
        MAP_KEY_TO_STRING(Num9, "9")
        MAP_KEY_TO_STRING(Escape, "Escape")
        MAP_KEY_TO_STRING(LControl, "LControl")
        MAP_KEY_TO_STRING(LShift, "LShift")
        MAP_KEY_TO_STRING(LAlt, "LAlt")
        MAP_KEY_TO_STRING(LSystem, "LSystem")
        MAP_KEY_TO_STRING(RControl, "RControl")
        MAP_KEY_TO_STRING(RShift, "RShift")
        MAP_KEY_TO_STRING(RAlt, "RAlt")
        MAP_KEY_TO_STRING(RSystem, "RSystem")
        MAP_KEY_TO_STRING(Menu, "Menu")
        MAP_KEY_TO_STRING(LBracket, "LBracket")
        MAP_KEY_TO_STRING(RBracket, "RBracket")
        MAP_KEY_TO_STRING(SemiColon, "SemiColon")
        MAP_KEY_TO_STRING(Comma, "Comma")
        MAP_KEY_TO_STRING(Period, "Period")
        MAP_KEY_TO_STRING(Quote, "Quote")
        MAP_KEY_TO_STRING(Slash, "Slash")
        MAP_KEY_TO_STRING(BackSlash, "BackSlash")
        MAP_KEY_TO_STRING(Tilde, "Tilde")
        MAP_KEY_TO_STRING(Equal, "Equal")
        MAP_KEY_TO_STRING(Dash, "Dash")
        MAP_KEY_TO_STRING(Space, "Space")
        MAP_KEY_TO_STRING(Return, "Return")
        MAP_KEY_TO_STRING(BackSpace, "BackSpace")
        MAP_KEY_TO_STRING(Tab, "Tab")
        MAP_KEY_TO_STRING(PageUp, "PageUp")
        MAP_KEY_TO_STRING(PageDown, "PageDown")
        MAP_KEY_TO_STRING(End, "End")
        MAP_KEY_TO_STRING(Home, "Home")
        MAP_KEY_TO_STRING(Insert, "Insert")
        MAP_KEY_TO_STRING(Delete, "Delete")
        MAP_KEY_TO_STRING(Add, "Add")
        MAP_KEY_TO_STRING(Subtract, "Subtract")
        MAP_KEY_TO_STRING(Multiply, "Multiply")
        MAP_KEY_TO_STRING(Divide, "Divide")
        MAP_KEY_TO_STRING(Left, "Left")
        MAP_KEY_TO_STRING(Right, "Right")
        MAP_KEY_TO_STRING(Up, "Up")
        MAP_KEY_TO_STRING(Down, "Down")
        MAP_KEY_TO_STRING(Numpad0, "Numpad0")
        MAP_KEY_TO_STRING(Numpad1, "Numpad1")
        MAP_KEY_TO_STRING(Numpad2, "Numpad2")
        MAP_KEY_TO_STRING(Numpad3, "Numpad3")
        MAP_KEY_TO_STRING(Numpad4, "Numpad4")
        MAP_KEY_TO_STRING(Numpad5, "Numpad5")
        MAP_KEY_TO_STRING(Numpad6, "Numpad6")
        MAP_KEY_TO_STRING(Numpad7, "Numpad7")
        MAP_KEY_TO_STRING(Numpad8, "Numpad8")
        MAP_KEY_TO_STRING(Numpad9, "Numpad9")
        MAP_KEY_TO_STRING(F1, "F1")
        MAP_KEY_TO_STRING(F2, "F2")
        MAP_KEY_TO_STRING(F3, "F3")
        MAP_KEY_TO_STRING(F4, "F4")
        MAP_KEY_TO_STRING(F5, "F5")
        MAP_KEY_TO_STRING(F6, "F6")
        MAP_KEY_TO_STRING(F7, "F7")
        MAP_KEY_TO_STRING(F8, "F8")
        MAP_KEY_TO_STRING(F9, "F9")
        MAP_KEY_TO_STRING(F10, "F10")
        MAP_KEY_TO_STRING(F11, "F11")
        MAP_KEY_TO_STRING(F12, "F12")
        MAP_KEY_TO_STRING(Pause, "Pause")
    default:
        return "Invalid";
    }
#undef MAP_KEY_TO_STRING
}

template <>
void data::serialize<Key>(Serializer& ser, const Key& obj, const char* name)
{
    ser.write(io::keyToString(obj), name);
}

Key io::stringToKey(const std::string& str)
{
#define MAP_STRING_TO_KEY(string, key)                                                                                 \
    if (str == (string))                                                                                               \
        return Key::key;
    MAP_STRING_TO_KEY("a", A)
    MAP_STRING_TO_KEY("b", B)
    MAP_STRING_TO_KEY("c", C)
    MAP_STRING_TO_KEY("d", D)
    MAP_STRING_TO_KEY("e", E)
    MAP_STRING_TO_KEY("f", F)
    MAP_STRING_TO_KEY("g", G)
    MAP_STRING_TO_KEY("h", H)
    MAP_STRING_TO_KEY("i", I)
    MAP_STRING_TO_KEY("j", J)
    MAP_STRING_TO_KEY("k", K)
    MAP_STRING_TO_KEY("l", L)
    MAP_STRING_TO_KEY("m", M)
    MAP_STRING_TO_KEY("n", N)
    MAP_STRING_TO_KEY("o", O)
    MAP_STRING_TO_KEY("p", P)
    MAP_STRING_TO_KEY("q", Q)
    MAP_STRING_TO_KEY("r", R)
    MAP_STRING_TO_KEY("s", S)
    MAP_STRING_TO_KEY("t", T)
    MAP_STRING_TO_KEY("u", U)
    MAP_STRING_TO_KEY("v", V)
    MAP_STRING_TO_KEY("w", W)
    MAP_STRING_TO_KEY("x", X)
    MAP_STRING_TO_KEY("y", Y)
    MAP_STRING_TO_KEY("z", Z)
    MAP_STRING_TO_KEY("0", Num0)
    MAP_STRING_TO_KEY("1", Num1)
    MAP_STRING_TO_KEY("2", Num2)
    MAP_STRING_TO_KEY("3", Num3)
    MAP_STRING_TO_KEY("4", Num4)
    MAP_STRING_TO_KEY("5", Num5)
    MAP_STRING_TO_KEY("6", Num6)
    MAP_STRING_TO_KEY("7", Num7)
    MAP_STRING_TO_KEY("8", Num8)
    MAP_STRING_TO_KEY("9", Num9)
    MAP_STRING_TO_KEY("Escape", Escape)
    MAP_STRING_TO_KEY("LControl", LControl)
    MAP_STRING_TO_KEY("LShift", LShift)
    MAP_STRING_TO_KEY("LAlt", LAlt)
    MAP_STRING_TO_KEY("LSystem", LSystem)
    MAP_STRING_TO_KEY("RControl", RControl)
    MAP_STRING_TO_KEY("RShift", RShift)
    MAP_STRING_TO_KEY("RAlt", RAlt)
    MAP_STRING_TO_KEY("RSystem", RSystem)
    MAP_STRING_TO_KEY("Menu", Menu)
    MAP_STRING_TO_KEY("LBracket", LBracket)
    MAP_STRING_TO_KEY("RBracket", RBracket)
    MAP_STRING_TO_KEY("SemiColon", SemiColon)
    MAP_STRING_TO_KEY("Comma", Comma)
    MAP_STRING_TO_KEY("Period", Period)
    MAP_STRING_TO_KEY("Quote", Quote)
    MAP_STRING_TO_KEY("Slash", Slash)
    MAP_STRING_TO_KEY("BackSlash", BackSlash)
    MAP_STRING_TO_KEY("Tilde", Tilde)
    MAP_STRING_TO_KEY("Equal", Equal)
    MAP_STRING_TO_KEY("Dash", Dash)
    MAP_STRING_TO_KEY("Space", Space)
    MAP_STRING_TO_KEY("Return", Return)
    MAP_STRING_TO_KEY("BackSpace", BackSpace)
    MAP_STRING_TO_KEY("Tab", Tab)
    MAP_STRING_TO_KEY("PageUp", PageUp)
    MAP_STRING_TO_KEY("PageDown", PageDown)
    MAP_STRING_TO_KEY("End", End)
    MAP_STRING_TO_KEY("Home", Home)
    MAP_STRING_TO_KEY("Insert", Insert)
    MAP_STRING_TO_KEY("Delete", Delete)
    MAP_STRING_TO_KEY("Add", Add)
    MAP_STRING_TO_KEY("Subtract", Subtract)
    MAP_STRING_TO_KEY("Multiply", Multiply)
    MAP_STRING_TO_KEY("Divide", Divide)
    MAP_STRING_TO_KEY("Left", Left)
    MAP_STRING_TO_KEY("Right", Right)
    MAP_STRING_TO_KEY("Up", Up)
    MAP_STRING_TO_KEY("Down", Down)
    MAP_STRING_TO_KEY("Numpad0", Numpad0)
    MAP_STRING_TO_KEY("Numpad1", Numpad1)
    MAP_STRING_TO_KEY("Numpad2", Numpad2)
    MAP_STRING_TO_KEY("Numpad3", Numpad3)
    MAP_STRING_TO_KEY("Numpad4", Numpad4)
    MAP_STRING_TO_KEY("Numpad5", Numpad5)
    MAP_STRING_TO_KEY("Numpad6", Numpad6)
    MAP_STRING_TO_KEY("Numpad7", Numpad7)
    MAP_STRING_TO_KEY("Numpad8", Numpad8)
    MAP_STRING_TO_KEY("Numpad9", Numpad9)
    MAP_STRING_TO_KEY("F1", F1)
    MAP_STRING_TO_KEY("F2", F2)
    MAP_STRING_TO_KEY("F3", F3)
    MAP_STRING_TO_KEY("F4", F4)
    MAP_STRING_TO_KEY("F5", F5)
    MAP_STRING_TO_KEY("F6", F6)
    MAP_STRING_TO_KEY("F7", F7)
    MAP_STRING_TO_KEY("F8", F8)
    MAP_STRING_TO_KEY("F9", F9)
    MAP_STRING_TO_KEY("F10", F10)
    MAP_STRING_TO_KEY("F11", F11)
    MAP_STRING_TO_KEY("F12", F12)
    MAP_STRING_TO_KEY("Pause", Pause)
    // else
    return Key::Invalid;
#undef MAP_STRING_TO_KEY
}

template <>
void data::deserialize<Key>(Deserializer& des, Key& obj)
{
    std::string str;
    des.read(str);
    obj = io::stringToKey(str);
}
