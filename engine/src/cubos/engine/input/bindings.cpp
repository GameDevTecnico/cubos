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

const Key& KeyWithModifier::getKey() const
{
    return mKey;
}

const Modifiers& KeyWithModifier::getModifiers() const
{
    return mModifiers;
}

Key& KeyWithModifier::getKey()
{
    return mKey;
}

Modifiers& KeyWithModifier::getModifiers()
{
    return mModifiers;
}

std::string KeyWithModifier::toString() const
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

const std::vector<KeyWithModifier>& Axis::getPositive() const
{
    return mPositive;
}

const std::vector<KeyWithModifier>& Axis::getNegative() const
{
    return mNegative;
}

std::vector<KeyWithModifier>& Axis::getPositive()
{
    return mPositive;
}

std::vector<KeyWithModifier>& Axis::getNegative()
{
    return mNegative;
}

float Axis::getValue() const
{
    return mValue;
}

void Axis::setValue(float value)
{
    if (std::abs(value) > 1.0f)
    {
        CUBOS_WARN("Axis value out of range: {}", value);
    }
    mValue = std::clamp(value, -1.0f, 1.0f);
}

std::string Axis::toString() const
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

const std::vector<KeyWithModifier>& Action::getKeys() const
{
    return mKeys;
}

std::vector<KeyWithModifier>& Action::getKeys()
{
    return mKeys;
}

bool Action::isPressed() const
{
    return mPressed;
}

void Action::setPressed(bool pressed)
{
    mPressed = pressed;
}

std::string Action::toString() const
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

const std::unordered_map<std::string, Action>& Bindings::getActions() const
{
    return mActions;
}

const std::unordered_map<std::string, Axis>& Bindings::getAxes() const
{
    return mAxes;
}

std::unordered_map<std::string, Action>& Bindings::getActions()
{
    return mActions;
}

std::unordered_map<std::string, Axis>& Bindings::getAxes()
{
    return mAxes;
}

std::string Bindings::toString() const
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
void cubos::core::data::serialize<Bindings>(Serializer& ser, const Bindings& obj, const char* name)
{
    ser.beginObject(name);
    ser.beginDictionary(obj.getActions().size(), "action");
    for (const auto& [actionName, action] : obj.getActions())
    {
        ser.write(action.getKeys(), actionName.c_str());
    }
    ser.endDictionary();
    ser.beginDictionary(obj.getAxes().size(), "axis");
    for (const auto& [axisName, axis] : obj.getAxes())
    {
        ser.beginDictionary(2, axisName.c_str());
        ser.write(axis.getPositive(), "pos");
        ser.write(axis.getNegative(), "neg");
        ser.endDictionary();
    }
    ser.endDictionary();
    ser.endObject();
}

template <>
void cubos::core::data::deserialize<Bindings>(Deserializer& des, Bindings& obj)
{
    des.beginObject();

    size_t actions_sz = des.beginDictionary();
    for (size_t i = 0; i < actions_sz; ++i)
    {
        std::string action;
        des.read(action);

        std::vector<KeyWithModifier> keys;
        des.read(keys);

        obj.getActions()[action] = Action(keys);
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
        std::vector<KeyWithModifier> pos;
        des.read(pos);

        des.read(tmp);
        CUBOS_ASSERT(tmp == "neg", "Expected 'neg' in axis dictionary");
        std::vector<KeyWithModifier> neg;
        des.read(neg);

        des.endDictionary();

        obj.getAxes()[axis] = Axis(pos, neg);
    }
    des.endDictionary();

    des.endObject();
}

template <>
void cubos::core::data::serialize<KeyWithModifier>(Serializer& ser, const KeyWithModifier& obj, const char* name)
{
    std::stringstream ss;

    Modifiers modifiers = obj.getModifiers();
    if ((modifiers & Modifiers::System) != Modifiers::None)
        ss << "D-";

    if ((modifiers & Modifiers::Alt) != Modifiers::None)
        ss << "M-";

    if ((modifiers & Modifiers::Shift) != Modifiers::None)
        ss << "S-";

    if ((modifiers & Modifiers::Control) != Modifiers::None)
        ss << "C-";

    ss << keyToString(obj.getKey());

    ser.writeString(ss.str().c_str(), name);
}

template <>
void cubos::core::data::deserialize<KeyWithModifier>(Deserializer& des, KeyWithModifier& obj)
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

    obj = KeyWithModifier(key, modifiers);
}