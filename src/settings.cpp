#include <cubos/settings.hpp>

cubos::Settings* cubos::Settings::_instance = nullptr;

cubos::Settings* cubos::Settings::GetInstance()
{
    if (_instance == nullptr)
    {
        _instance = new Settings();
    }
    return _instance;
}

void cubos::Settings::clear()
{
    _values.clear();
}

void cubos::Settings::setString(const std::string& key, const std::string& value)
{
    _values[key] = value;
}

std::string cubos::Settings::getString(const std::string& key, const std::string& defaultValue)
{
    if (_values.count(key) == 0)
        return defaultValue;

    return _values[key];
}