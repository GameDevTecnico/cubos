#ifndef CUBOS_SETTINGS_HPP
#define CUBOS_SETTINGS_HPP
#include <memory>
#include <string>
#include <unordered_map>

namespace cubos
{

    class Settings
    {
    protected:
        Settings()
        {
        }
        static Settings* _instance;

    public:
        Settings(Settings& settings) = delete;
        void operator=(const Settings&) = delete;
        static Settings* GetInstance();

        void clear();

        void setString(const std::string& key, const std::string& value);
        std::string getString(const std::string& key, const std::string& defaultValue);

    private:
        std::unordered_map<std::string, std::string> _values;
    };

} // namespace cubos

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

#endif // CUBOS_SETTINGS_HPP