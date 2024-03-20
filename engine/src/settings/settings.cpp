#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/settings/settings.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Settings)
{
    return core::ecs::TypeBuilder<Settings>("cubos::engine::Settings").build();
}

template <>
void cubos::core::data::old::serialize<Settings>(Serializer& ser, const Settings& obj, const char* name)
{
    ser.write(obj.getValues(), name);
}

template <>
void cubos::core::data::old::deserialize<Settings>(Deserializer& des, Settings& obj)
{
    des.read(obj.getValues());
}

void Settings::clear()
{
    mValues.clear();
}

void Settings::setBool(const std::string& key, bool value)
{
    mValues[key] = value ? "true" : "false";
}

bool Settings::getBool(const std::string& key, bool defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, defaultValue ? "true" : "false"});
    }

    return mValues.at(key) == "true";
}

void Settings::setString(const std::string& key, const std::string& value)
{
    mValues[key] = value;
}

std::string Settings::getString(const std::string& key, const std::string& defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, defaultValue});
    }

    return mValues.at(key);
}

void Settings::setInteger(const std::string& key, int value)
{
    mValues[key] = std::to_string(value);
}

int Settings::getInteger(const std::string& key, int defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, std::to_string(defaultValue)});
    }

    try
    {
        return std::stoi(mValues.at(key));
    }
    catch (...)
    {
        return defaultValue;
    }
}

void Settings::setDouble(const std::string& key, double value)
{
    mValues[key] = std::to_string(value);
}

double Settings::getDouble(const std::string& key, double defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, std::to_string(defaultValue)});
    }

    try
    {
        return std::stod(mValues.at(key));
    }
    catch (...)
    {
        return defaultValue;
    }
}

void Settings::merge(const Settings& settingsToMerge)
{
    for (const auto& value : settingsToMerge.mValues)
    {
        this->setString(value.first, value.second);
    }
}

const std::unordered_map<std::string, std::string>& Settings::getValues() const
{
    return mValues;
}

std::unordered_map<std::string, std::string>& Settings::getValues()
{
    return mValues;
}
