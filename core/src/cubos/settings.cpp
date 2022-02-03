#include <cubos/settings.hpp>

using namespace cubos;

Settings Settings::global = Settings();

void Settings::clear()
{
    this->values.clear();
}

void Settings::setString(const std::string& key, const std::string& value)
{
    this->values[key] = value;
}

std::string Settings::getString(const std::string& key, const std::string& defaultValue)
{
    if (this->values.count(key) == 0)
        return defaultValue;

    return this->values[key];
}

void Settings::merge(const Settings& settingsToMerge)
{
    for (auto it = settingsToMerge.values.begin(); it != settingsToMerge.values.end(); it++)
    {
        this->setString(it->first, it->second);
    }
}

void Settings::serialize(cubos::memory::Serializer& serializer) const
{
    serializer.beginDictionary(this->values.size(), "settings");
    for (auto entry : this->values)
    {
        serializer.write(entry.first, "key");
        serializer.write(entry.second, "value");
    }
    serializer.endDictionary();
}

void Settings::deserialize(cubos::memory::Deserializer& deserializer)
{
    size_t settings_size = deserializer.beginDictionary();
    for (int i = 0; i < settings_size; i++)
    {
        std::string setting_key;
        std::string setting_value;
        deserializer.read(setting_key);
        deserializer.read(setting_value);

        this->setString(setting_key, setting_value);
    }
    deserializer.endDictionary();
}