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

        /**
         * @brief Clears all the settings
         *
         * @return * void
         */
        static void clear();

        /**
         * @brief Defines a new string setting.
         *        If the setting already exists, overrides its value with the new value.
         *
         * @param key the setting's key
         * @param value the setting's value
         */
        static void setString(const std::string& key, const std::string& value);

        /**
         * @brief Retrieves the string settings with a specific key.
         *        If no setting exists with such key, returns the default value.
         *
         * @param key the setting's key
         * @param defaultValue the value returned when not setting with the provided key exists
         * @return std::string the setting's value
         */
        static std::string getString(const std::string& key, const std::string& defaultValue);

    private:
        std::unordered_map<std::string, std::string> _values;
        static Settings* GetInstance();
    };

} // namespace cubos

#endif // CUBOS_SETTINGS_HPP