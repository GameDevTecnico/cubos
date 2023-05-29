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
        result += "C-";
    if ((modifiers & Modifiers::Shift) != Modifiers::None)
        result += "S-";
    if ((modifiers & Modifiers::Alt) != Modifiers::None)
        result += "M-";
    if ((modifiers & Modifiers::System) != Modifiers::None)
        result += "D-";

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
        result |= Modifiers::Control;
    if (str.find("S-") != std::string::npos)
        result |= Modifiers::Shift;
    if (str.find("M-") != std::string::npos)
        result |= Modifiers::Alt;
    if (str.find("D-") != std::string::npos)
        result |= Modifiers::System;

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
    // TODO: Add more keys.
    switch (key)
    {
    case Key::A:
        return "a";
    case Key::B:
        return "b";
    case Key::C:
        return "c";
    case Key::D:
        return "d";
    case Key::E:
        return "e";
    case Key::F:
        return "f";
    case Key::G:
        return "g";
    case Key::H:
        return "h";
    case Key::I:
        return "i";
    case Key::J:
        return "j";
    case Key::K:
        return "k";
    case Key::L:
        return "l";
    case Key::M:
        return "m";
    case Key::N:
        return "n";
    case Key::O:
        return "o";
    case Key::P:
        return "p";
    case Key::Q:
        return "q";
    case Key::R:
        return "r";
    case Key::S:
        return "s";
    case Key::T:
        return "t";
    case Key::U:
        return "u";
    case Key::V:
        return "v";
    case Key::W:
        return "w";
    case Key::X:
        return "x";
    case Key::Y:
        return "y";
    case Key::Z:
        return "z";
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

template <>
void data::serialize<Key>(Serializer& ser, const Key& obj, const char* name)
{
    ser.write(io::keyToString(obj), name);
}

Key io::stringToKey(const std::string& str)
{
    // TODO: Add more keys.
    if (str == "a")
        return Key::A;
    else if (str == "b")
        return Key::B;
    else if (str == "c")
        return Key::C;
    else if (str == "d")
        return Key::D;
    else if (str == "e")
        return Key::E;
    else if (str == "f")
        return Key::F;
    else if (str == "g")
        return Key::G;
    else if (str == "h")
        return Key::H;
    else if (str == "i")
        return Key::I;
    else if (str == "j")
        return Key::J;
    else if (str == "k")
        return Key::K;
    else if (str == "l")
        return Key::L;
    else if (str == "m")
        return Key::M;
    else if (str == "n")
        return Key::N;
    else if (str == "o")
        return Key::O;
    else if (str == "p")
        return Key::P;
    else if (str == "q")
        return Key::Q;
    else if (str == "r")
        return Key::R;
    else if (str == "s")
        return Key::S;
    else if (str == "t")
        return Key::T;
    else if (str == "u")
        return Key::U;
    else if (str == "v")
        return Key::V;
    else if (str == "w")
        return Key::W;
    else if (str == "x")
        return Key::X;
    else if (str == "y")
        return Key::Y;
    else if (str == "z")
        return Key::Z;
    else if (str == "Up")
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

template <>
void data::deserialize<Key>(Deserializer& des, Key& obj)
{
    std::string str;
    des.read(str);
    obj = io::stringToKey(str);
}
