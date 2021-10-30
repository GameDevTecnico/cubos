#include <cubos/settings.hpp>

using namespace cubos;

Settings& Settings::getInstance()
{
    static Settings instance;
    return instance;
}

void Settings::clear()
{
    auto& settings = Settings::getInstance();
    settings.values.clear();
}

void Settings::setString(const std::string& key, const std::string& value)
{
    auto& settings = Settings::getInstance();
    settings.values[key] = value;
}

std::string Settings::getString(const std::string& key, const std::string& defaultValue)
{
    auto& settings = Settings::getInstance();
    if (settings.values.count(key) == 0)
        return defaultValue;

    return settings.values[key];
}
