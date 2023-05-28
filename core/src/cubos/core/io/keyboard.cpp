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
void cubos::core::data::serialize<Modifiers>(Serializer& ser, const Modifiers& obj, const char* name)
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
void cubos::core::data::deserialize<Modifiers>(Deserializer& des, Modifiers& obj)
{
    std::string str;
    des.read(str);
    obj = io::stringToModifiers(str);
}

std::string io::keyToString(Key key)
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

template <>
void cubos::core::data::serialize<Key>(Serializer& ser, const Key& obj, const char* name)
{
    ser.write(io::keyToString(obj), name);
}

Key io::stringToKey(const std::string& str)
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

template <>
void cubos::core::data::deserialize<Key>(Deserializer& des, Key& obj)
{
    std::string str;
    des.read(str);
    obj = io::stringToKey(str);
}