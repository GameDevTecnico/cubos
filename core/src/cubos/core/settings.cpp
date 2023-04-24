#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/settings.hpp>

using namespace cubos::core;

template <>
void data::serialize<Settings>(Serializer& ser, const Settings& settings, const char* name)
{
    ser.write(settings.getValues(), name);
}

template <>
void data::deserialize<Settings>(Deserializer& des, Settings& settings)
{
    des.read(settings.getValues());
}

void Settings::clear()
{
    this->values.clear();
}

void Settings::setBool(const std::string& key, const bool& value)
{
    this->values[key] = value ? "true" : "false";
}

bool Settings::getBool(const std::string& key, const bool& defaultValue) const
{
    if (this->values.count(key) == 0)
        return defaultValue;

    return this->values.at(key) == "true" ? true : false;
}

void Settings::setString(const std::string& key, const std::string& value)
{
    this->values[key] = value;
}

std::string Settings::getString(const std::string& key, const std::string& defaultValue) const
{
    if (this->values.count(key) == 0)
        return defaultValue;

    return this->values.at(key);
}

void Settings::setInteger(const std::string& key, const int value)
{
    this->values[key] = std::to_string(value);
}

int Settings::getInteger(const std::string& key, const int defaultValue) const
{
    if (this->values.count(key) == 0)
        return defaultValue;

    return std::stoi(this->values.at(key));
}

void Settings::setDouble(const std::string& key, const double value)
{
    this->values[key] = std::to_string(value);
}

double Settings::getDouble(const std::string& key, const double defaultValue) const
{
    if (this->values.count(key) == 0)
        return defaultValue;

    return std::stod(this->values.at(key));
}

void Settings::merge(const Settings& settingsToMerge)
{
    for (const auto& value : settingsToMerge.values)
    {
        this->setString(value.first, value.second);
    }
}

const std::unordered_map<std::string, std::string>& Settings::getValues() const
{
    return this->values;
}

std::unordered_map<std::string, std::string>& Settings::getValues()
{
    return this->values;
}
