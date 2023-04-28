#ifndef CUBOS_CORE_SETTINGS_HPP
#define CUBOS_CORE_SETTINGS_HPP

#include <string>
#include <unordered_map>

namespace cubos::core
{
    /// Object used to manage settings.
    class Settings final
    {
    public:
        Settings() = default;
        ~Settings() = default;

        /// Clears all the settings.
        void clear();

        /// Defines a new bool setting.
        /// If the setting already exists, overrides its value with the new value.
        /// @param key The setting's key.
        /// @param value The setting's value.
        void setBool(const std::string& key, const bool& value);

        /// Retrieves the bool settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key The setting's key.
        /// @param defaultValue The value returned when not setting with the provided key exists.
        /// @return The setting's value.
        bool getBool(const std::string& key, const bool& defaultValue) const;

        /// Defines a new string setting.
        /// If the setting already exists, overrides its value with the new value.
        /// @param key The setting's key.
        /// @param value The setting's value.
        void setString(const std::string& key, const std::string& value);

        /// Retrieves the string settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key The setting's key.
        /// @param defaultValue The value returned when not setting with the provided key exists.
        /// @return The setting's value.
        std::string getString(const std::string& key, const std::string& defaultValue) const;

        /// Defines a new integer setting.
        /// If the setting already exists, overrides its value with the new value.
        /// @param key The setting's key.
        /// @param value The setting's value.
        void setInteger(const std::string& key, int value);

        /// Retrieves the integer settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key The setting's key.
        /// @param defaultValue The value returned when not setting with the provided key exists.
        /// @return The setting's value.
        int getInteger(const std::string& key, int defaultValue) const;

        /// Defines a new double setting.
        /// If the setting already exists, overrides its value with the new value.
        /// @param key The setting's key.
        /// @param value The setting's value.
        void setDouble(const std::string& key, double value);

        /// Retrieves the double settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key The setting's key.
        /// @param defaultValue The value returned when not setting with the provided key exists.
        /// @return The setting's value.
        double getDouble(const std::string& key, double defaultValue) const;

        /// Merges the settings from settingsToMerge.
        /// If a setting from settingsToMerge already exists, overrides its value. @see setString.
        /// @param settingsToMerge The settings to be merged to this instance.
        void merge(const Settings& settingsToMerge);

        /// @returns The underlying unordered_map with the settings.
        const std::unordered_map<std::string, std::string>& getValues() const;

        /// @returns The underlying unordered_map with the settings.
        std::unordered_map<std::string, std::string>& getValues();

    private:
        std::unordered_map<std::string, std::string> mValues;
    };
} // namespace cubos::core

#endif // CUBOS_CORE_SETTINGS_HPP
