#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/settings.hpp>

using namespace cubos::core;

template <>
void data::serialize<Settings>(Serializer& ser, const Settings& obj, const char* name)
{
    ser.write(obj.getValues(), name);
}

template <>
void data::deserialize<Settings>(Deserializer& des, Settings& obj)
{
    des.read(obj.getValues());
}

void Settings::clear()
{
    mValues.clear();
}

void Settings::setBool(const std::string& key, const bool& value)
{
    mValues[key] = value ? "true" : "false";
}

bool Settings::getBool(const std::string& key, const bool& defaultValue) const
{
    if (!mValues.contains(key))
    {
        return defaultValue;
    }

    return mValues.at(key) == "true";
}

void Settings::setString(const std::string& key, const std::string& value)
{
    mValues[key] = value;
}

std::string Settings::getString(const std::string& key, const std::string& defaultValue) const
{
    if (!mValues.contains(key))
    {
        return defaultValue;
    }

    return mValues.at(key);
}

void Settings::setInteger(const std::string& key, const int value)
{
    mValues[key] = std::to_string(value);
}

int Settings::getInteger(const std::string& key, const int defaultValue) const
{
    if (!mValues.contains(key))
    {
        return defaultValue;
    }

    return std::stoi(mValues.at(key));
}

void Settings::setDouble(const std::string& key, const double value)
{
    mValues[key] = std::to_string(value);
}

double Settings::getDouble(const std::string& key, const double defaultValue) const
{
    if (!mValues.contains(key))
    {
        return defaultValue;
    }

    return std::stod(mValues.at(key));
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
