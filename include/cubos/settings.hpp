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

#endif // CUBOS_SETTINGS_HPP