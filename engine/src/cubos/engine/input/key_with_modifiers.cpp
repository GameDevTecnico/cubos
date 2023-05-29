#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/key_with_modifiers.hpp>

using cubos::core::io::Key;
using cubos::core::io::keyToString;
using cubos::core::io::Modifiers;
using cubos::core::io::modifiersToString;
using cubos::core::io::stringToKey;
using cubos::core::io::stringToModifiers;

using namespace cubos::engine;

const Key& KeyWithModifiers::key() const
{
    return mKey;
}

const Modifiers& KeyWithModifiers::modifiers() const
{
    return mModifiers;
}

Key& KeyWithModifiers::key()
{
    return mKey;
}

Modifiers& KeyWithModifiers::modifiers()
{
    return mModifiers;
}

bool KeyWithModifiers::pressed() const
{
    return mPressed;
}

void KeyWithModifiers::pressed(bool pressed)
{
    mPressed = pressed;
}

template <>
void cubos::core::data::serialize<KeyWithModifiers>(Serializer& ser, const KeyWithModifiers& obj, const char* name)
{
    ser.write(modifiersToString(obj.modifiers()) + keyToString(obj.key()), name);
}

template <>
void cubos::core::data::deserialize<KeyWithModifiers>(Deserializer& des, KeyWithModifiers& obj)
{
    std::string str;
    des.readString(str);

    std::size_t split = str.find_last_of('-');
    if (split == std::string::npos)
    {
        obj = KeyWithModifiers{stringToKey(str), Modifiers::None};
    }
    else
    {
        obj = KeyWithModifiers{stringToKey(str.substr(split + 1)), stringToModifiers(str.substr(0, split + 1))};
    }
}
