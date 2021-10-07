#include <cubos/settings.hpp>

using namespace cubos;

Settings* Settings::_instance = nullptr;

Settings* Settings::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new Settings();
    }
    return _instance;
}

void Settings::clear()
{
    auto settings = Settings::getInstance();
    settings->_values.clear();
}

void Settings::setString(const std::string& key, const std::string& value)
{
    auto settings = Settings::getInstance();
    settings->_values[key] = value;
}

std::string Settings::getString(const std::string& key, const std::string& defaultValue)
{
    auto settings = Settings::getInstance();
    if (settings->_values.count(key) == 0)
        return defaultValue;

    return settings->_values[key];
}
