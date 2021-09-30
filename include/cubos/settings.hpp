#ifndef CUBOS_SETTINGS_HPP
#define CUBOS_SETTINGS_HPP
#include <string>
#include <map>
#include <any>

namespace cubos
{

    class Settings
    {
    protected:
        Settings();
        static Settings* _instance;

    public:
        Settings(Settings& settings) = delete;
        void operator=(const Settings&) = delete;
        static Settings* GetInstance();

        template <typename T> void set(const std::string& key, T value);
        template <typename T> T get(const std::string& key, T defaultValue);

    private:
        std::map<std::string, std::any> _values;
    };

} // namespace cubos

cubos::Settings* cubos::Settings::GetInstance()
{
    if (_instance == nullptr)
    {
        _instance = new Settings();
    }
    return _instance;
}

#endif