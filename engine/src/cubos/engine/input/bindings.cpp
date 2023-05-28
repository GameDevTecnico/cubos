#include <sstream>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/bindings.hpp>

using cubos::core::data::Deserializer;
using cubos::core::data::Serializer;
using cubos::core::io::Key;
using cubos::core::io::keyToString;
using cubos::core::io::Modifiers;
using cubos::core::io::stringToKey;
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

std::string KeyWithModifiers::toString() const
{
    std::stringstream ss;
    ss << keyToString(mKey);

    if ((mModifiers & Modifiers::Shift) != Modifiers::None)
    {
        ss << "+Shift";
    }

    if ((mModifiers & Modifiers::Control) != Modifiers::None)
    {
        ss << "+Control";
    }

    if ((mModifiers & Modifiers::Alt) != Modifiers::None)
    {
        ss << "+Alt";
    }

    if ((mModifiers & Modifiers::System) != Modifiers::None)
    {
        ss << "+System";
    }

    return ss.str();
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

std::string InputAxis::toString() const
{
    std::stringstream ss;
    ss << "Positive: { ";
    for (const auto& key : mPositive)
    {
        ss << key.toString() << ", ";
    }
    ss << "}, Negative: { ";
    for (const auto& key : mNegative)
    {
        ss << key.toString() << ", ";
    }
    ss << "}";
    return ss.str();
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

std::string InputAction::toString() const
{
    std::stringstream ss;
    ss << "Keys: { ";
    for (const auto& key : mKeys)
    {
        ss << key.toString() << ", ";
    }
    ss << "}";
    return ss.str();
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

std::string InputBindings::toString() const
{
    std::stringstream ss;
    ss << "Actions: { ";
    for (const auto& [name, action] : mActions)
    {
        ss << name << ": { " << action.toString() << "}, ";
    }
    ss << "}, Axes: { ";
    for (const auto& [name, axis] : mAxes)
    {
        ss << name << ": { " << axis.toString() << "}, ";
    }
    ss << "}";
    return ss.str();
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
    std::stringstream ss;

    Modifiers modifiers = obj.modifiers();
    if ((modifiers & Modifiers::System) != Modifiers::None)
        ss << "D-";

    if ((modifiers & Modifiers::Alt) != Modifiers::None)
        ss << "M-";

    if ((modifiers & Modifiers::Shift) != Modifiers::None)
        ss << "S-";

    if ((modifiers & Modifiers::Control) != Modifiers::None)
        ss << "C-";

    ss << keyToString(obj.key());

    ser.writeString(ss.str().c_str(), name);
}

template <>
void cubos::core::data::deserialize<KeyWithModifiers>(Deserializer& des, KeyWithModifiers& obj)
{
    std::string str;
    des.readString(str);
    std::stringstream ss(str);

    Modifiers modifiers = Modifiers::None;

    std::string token;
    while (std::getline(ss, token, '-') && !ss.eof())
    {
        CUBOS_ASSERT(token.size() == 1, "Invalid key format: Modifier must be a single character");

        switch (token[0])
        {
        case 'D':
            modifiers |= Modifiers::System;
            break;
        case 'M':
            modifiers |= Modifiers::Alt;
            break;
        case 'S':
            modifiers |= Modifiers::Shift;
            break;
        case 'C':
            modifiers |= Modifiers::Control;
            break;
        default:
            CUBOS_ERROR("Invalid key format: {} is not a valid modifier", token[0]);
        }
    }

    Key key = stringToKey(token);

    CUBOS_ASSERT(ss.eof(), "Invalid key format: Key must be the last part of the string");

    obj = KeyWithModifiers(key, modifiers);
}