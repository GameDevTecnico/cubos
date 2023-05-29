#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

#include <cubos/engine/input/bindings.hpp>

using cubos::core::io::Key;
using cubos::core::io::keyToString;
using cubos::core::io::Modifiers;
using cubos::core::io::modifiersToString;
using cubos::core::io::stringToKey;
using cubos::core::io::stringToModifiers;
using namespace cubos::engine;

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

    std::size_t actions_sz = des.beginDictionary();
    for (std::size_t i = 0; i < actions_sz; ++i)
    {
        std::string action;
        des.read(action);
        des.read(obj.actions()[action].keys());
    }
    des.endDictionary();

    std::size_t axes_sz = des.beginDictionary();
    for (std::size_t i = 0; i < axes_sz; ++i)
    {
        std::string axis;
        des.read(axis);
        des.beginObject();
        des.read(obj.axes()[axis].positive());
        des.read(obj.axes()[axis].negative());
        des.endObject();
    }
    des.endDictionary();

    des.endObject();
}

// This is a specialization of the serialize and deserialize functions for std::pair<Key, Modifiers>.
// Overloading these functions allows for human-readable serialization of the keybindings.

template <>
void cubos::core::data::serialize<Key, Modifiers>(Serializer& ser, const std::pair<Key, Modifiers>& obj,
                                                  const char* name)
{
    ser.write(modifiersToString(obj.second) + keyToString(obj.first), name);
}

template <>
void cubos::core::data::deserialize<Key, Modifiers>(Deserializer& des, std::pair<Key, Modifiers>& obj)
{
    std::string str;
    des.readString(str);

    std::size_t split = str.find_last_of('-');
    if (split == std::string::npos)
    {
        obj = {stringToKey(str), Modifiers::None};
    }
    else
    {
        obj = {stringToKey(str.substr(split + 1)), stringToModifiers(str.substr(0, split + 1))};
    }
}
