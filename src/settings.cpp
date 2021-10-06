#include <cubos/settings.hpp>

cubos::Settings* cubos::Settings::_instance = nullptr;

cubos::Settings* cubos::Settings::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new Settings();
    }
    return _instance;
}

void cubos::Settings::clear()
{
    cubos::Settings* settings = Settings::getInstance();
    settings->_values.clear();
}

void cubos::Settings::setString(const std::string& key, const std::string& value)
{
    cubos::Settings* settings = Settings::getInstance();
    settings->_values[key] = value;
}

std::string cubos::Settings::getString(const std::string& key, const std::string& defaultValue)
{
    cubos::Settings* settings = Settings::getInstance();
    if (settings->_values.count(key) == 0)
        return defaultValue;

    return settings->_values[key];
}