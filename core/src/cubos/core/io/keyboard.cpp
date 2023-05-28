#include <cubos/core/io/keyboard.hpp>

using cubos::core::io::Key;
using namespace cubos::core;

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