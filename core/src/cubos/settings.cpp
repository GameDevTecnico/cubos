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
