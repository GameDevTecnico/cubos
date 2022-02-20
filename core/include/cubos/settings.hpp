#ifndef CUBOS_SETTINGS_HPP
#define CUBOS_SETTINGS_HPP
#include <memory>
#include <string>
#include <unordered_map>
#include <cubos/memory/serializer.hpp>
#include <cubos/memory/deserializer.hpp>

namespace cubos
{
    /// Class used to manage settings.
    class Settings
    {
    public:
        /// Clears all the settings.
        void clear();

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
        void setInteger(const std::string& key, const int value);

        /// Retrieves the integer settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key The setting's key.
        /// @param defaultValue The value returned when not setting with the provided key exists.
        /// @return The setting's value.
        int getInteger(const std::string& key, const int defaultValue) const;

        /// Defines a new double setting.
        /// If the setting already exists, overrides its value with the new value.
        /// @param key The setting's key.
        /// @param value The setting's value.
        void setDouble(const std::string& key, const double value);

        /// Retrieves the double settings with a specific key.
        /// If no setting exists with such key, returns the default value.
        /// @param key The setting's key.
        /// @param defaultValue The value returned when not setting with the provided key exists.
        /// @return The setting's value.
        double getDouble(const std::string& key, const double defaultValue) const;

        /// Merges the settings from settingsToMerge.
        /// If a setting from settingsToMerge already exists, overrides its value. @see setString.
        /// @param settingsToMerge The settings to be merged to this instance.
        void merge(const Settings& settingsToMerge);

        static Settings global; ///< The Global Instance of Settings.

        /// Serializes the settings.
        /// @param serializer The serializer to use.
        void serialize(cubos::memory::Serializer& serializer) const;

        /// Deserializes the settings.
        /// @param deserializer The deserializer to use.
        void deserialize(cubos::memory::Deserializer& deserializer);

    private:
        std::unordered_map<std::string, std::string> values;
    };

} // namespace cubos

#endif // CUBOS_SETTINGS_HPP
