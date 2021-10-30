#ifndef CUBOS_SETTINGS_HPP
#define CUBOS_SETTINGS_HPP
#include <memory>
#include <string>
#include <unordered_map>

namespace cubos
{
    /// Singleton used to manage settings.
    class Settings
    {
    public:
        Settings(Settings&) = delete;
        void operator=(const Settings&) = delete;

        /// Clears all the settings.
        static void clear();

        /// Defines a new string setting.
        /// If the setting already exists, overrides its value with the new value.
        /// @param key The setting's key.
        /// @param value The setting's value.
        static void setString(const std::string& key, const std::string& value);

        /// Retrieves the string settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key The setting's key.
        /// @param defaultValue The value returned when not setting with the provided key exists.
        /// @return The setting's value.
        static std::string getString(const std::string& key, const std::string& defaultValue);

    private:
        Settings() = default;
        static Settings& getInstance();
        
        std::unordered_map<std::string, std::string> values;
    };

} // namespace cubos

#endif // CUBOS_SETTINGS_HPP
