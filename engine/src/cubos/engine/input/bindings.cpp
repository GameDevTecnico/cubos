#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/bindings.hpp>

using cubos::core::data::Deserializer;
using cubos::core::data::Serializer;
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

const std::vector<KeyWithModifiers>& InputAxis::positive() const
{
    return mPositive;
}

const std::vector<KeyWithModifiers>& InputAxis::negative() const
{
    return mNegative;
}

std::vector<KeyWithModifiers>& InputAxis::positive()
{
    return mPositive;
}

std::vector<KeyWithModifiers>& InputAxis::negative()
{
    return mNegative;
}

float InputAxis::value() const
{
    return mValue;
}

void InputAxis::value(float value)
{
    if (std::abs(value) > 1.0f)
    {
        CUBOS_WARN("Axis value out of range: {}", value);
    }
    mValue = std::clamp(value, -1.0f, 1.0f);
}

const std::vector<KeyWithModifiers>& InputAction::keys() const
{
    return mKeys;
}

std::vector<KeyWithModifiers>& InputAction::keys()
{
    return mKeys;
}

bool InputAction::pressed() const
{
    return mPressed;
}

void InputAction::pressed(bool pressed)
{
    mPressed = pressed;
}

const std::unordered_map<std::string, InputAction>& InputBindings::actions() const
{
    return mActions;
}

const std::unordered_map<std::string, InputAxis>& InputBindings::axes() const
{
    return mAxes;
}

std::unordered_map<std::string, InputAction>& InputBindings::actions()
{
    return mActions;
}

std::unordered_map<std::string, InputAxis>& InputBindings::axes()
{
    return mAxes;
}

template <>
void cubos::core::data::serialize<InputBindings>(Serializer& ser, const InputBindings& obj, const char* name)
{
    ser.beginObject(name);
    ser.beginDictionary(obj.actions().size(), "actions");
    for (const auto& [actionName, action] : obj.actions())
    {
        ser.write(actionName, nullptr);
        ser.write(action.keys(), nullptr);
    }
    ser.endDictionary();
    ser.beginDictionary(obj.axes().size(), "axes");
    for (const auto& [axisName, axis] : obj.axes())
    {
        ser.write(axisName, nullptr);
        ser.beginObject(nullptr);
        ser.write(axis.positive(), "pos");
        ser.write(axis.negative(), "neg");
        ser.endObject();
    }
    ser.endDictionary();
    ser.endObject();
}

template <>
void cubos::core::data::deserialize<InputBindings>(Deserializer& des, InputBindings& obj)
{
    des.beginObject();

    size_t actions_sz = des.beginDictionary();
    for (size_t i = 0; i < actions_sz; ++i)
    {
        std::string action;
        des.read(action);

        std::vector<KeyWithModifiers> keys;
        des.read(keys);

        obj.actions()[action] = InputAction(keys);
    }
    des.endDictionary();

    size_t axes_sz = des.beginDictionary();
    for (size_t i = 0; i < axes_sz; ++i)
    {
        std::string axis;
        des.read(axis);

        CUBOS_ASSERT(des.beginDictionary() == 2, "Expected 2 elements in axis dictionary");

        std::string tmp;
        des.read(tmp);
        CUBOS_ASSERT(tmp == "pos", "Expected 'pos' in axis dictionary");
        std::vector<KeyWithModifiers> pos;
        des.read(pos);

        des.read(tmp);
        CUBOS_ASSERT(tmp == "neg", "Expected 'neg' in axis dictionary");
        std::vector<KeyWithModifiers> neg;
        des.read(neg);

        des.endDictionary();

        obj.axes()[axis] = InputAxis(pos, neg);
    }
    des.endDictionary();

    des.endObject();
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

    size_t split = str.find_last_of('-');
    if (split == std::string::npos)
    {
        obj = KeyWithModifiers(stringToKey(str), Modifiers::None);
    }
    else
    {
        obj = KeyWithModifiers(stringToKey(str.substr(split + 1)), stringToModifiers(str.substr(0, split + 1)));
    }
}