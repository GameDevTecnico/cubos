#ifndef CUBOS_SETTINGS_HPP
#define CUBOS_SETTINGS_HPP
#include <memory>
#include <string>
#include <unordered_map>

namespace cubos
{

    class Settings
    {
    public:
        Settings(Settings& settings) = delete;
        void operator=(const Settings&) = delete;

        /// Clears all the settings
        static void clear();

        /// Defines a new string setting.
        /// If the setting already exists, overrides its value with the new value.
        /// @param key the setting's key
        /// @param value the setting's value
        static void setString(const std::string& key, const std::string& value);

        /// Retrieves the string settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key the setting's key
        /// @param defaultValue the value returned when not setting with the provided key exists
        /// @return std::string the setting's value
        static std::string getString(const std::string& key, const std::string& defaultValue);

    private:
        Settings() = default;
        static Settings* instance;
        std::unordered_map<std::string, std::string> values;
        static Settings* getInstance();
    };

} // namespace cubos

#endif // CUBOS_SETTINGS_HPP
