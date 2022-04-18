#include <string>
#include <cubos/core/settings.hpp>

using namespace cubos::core;

Settings Settings::global = Settings();

void Settings::clear()
{
    this->values.clear();
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
    for (auto it = settingsToMerge.values.begin(); it != settingsToMerge.values.end(); it++)
    {
        this->setString(it->first, it->second);
    }
}

void Settings::serialize(cubos::core::memory::Serializer& serializer) const
{
    serializer.write(this->values, "values");
}

void Settings::deserialize(cubos::core::memory::Deserializer& deserializer)
{
    deserializer.read(this->values);
}
