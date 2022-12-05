#ifndef CUBOS_CORE_SETTINGS_HPP
#define CUBOS_CORE_SETTINGS_HPP

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>

namespace cubos::core
{
    class Settings;

    namespace data
    {
        /// Serializes settings.
        /// @param serializer The serializer to use.
        /// @param settings The settings to serialize.
        /// @param name The name of the settings.
        void serialize(Serializer& serializer, const Settings& settings, const char* name);

        /// Deserializes settings.
        /// @param deserializer The deserializer to use.
        /// @param settings The settings to deserialize.
        void deserialize(Deserializer& deserializer, Settings& settings);
    } // namespace data

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

    private:
        friend void data::serialize(data::Serializer&, const Settings&, const char*);
        friend void data::deserialize(data::Deserializer&, Settings&);

        std::unordered_map<std::string, std::string> values;
    };
} // namespace cubos::core

#endif // CUBOS_CORE_SETTINGS_HPP
