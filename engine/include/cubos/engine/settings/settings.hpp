/// @file
/// @brief Class @ref cubos::engine::Settings.
/// @ingroup engine

#pragma once

#include <string>
#include <unordered_map>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Stores settings as key-value pairs and provides methods to retrieve them.
    /// @ingroup engine
    class CUBOS_ENGINE_API Settings final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Clears all the settings.
        void clear();

        /// @brief Defines a new boolean setting.
        ///
        /// If the setting already exists, overwrites its value.
        ///
        /// @param key Key.
        /// @param value Value.
        void setBool(const std::string& key, bool value);

        /// @brief Retrieves the bool setting with the given @p key.
        ///
        /// If no setting exists with such key, sets it to @p defaultValue and returns it.
        /// If the setting exists but its value is not "true", returns false.
        ///
        /// @param key Key.
        /// @param defaultValue Default value.
        /// @return Current value.
        bool getBool(const std::string& key, bool defaultValue);

        /// @brief Defines a new string setting.
        ///
        /// If the setting already exists, overwrites its value.
        ///
        /// @param key Key.
        /// @param value Value.
        void setString(const std::string& key, const std::string& value);

        /// @brief Retrieves the string setting with the given @p key.
        ///
        /// If no setting exists with such key, sets it to @p defaultValue and returns it.
        ///
        /// @param key Key.
        /// @param defaultValue Default value.
        /// @return Current value.
        std::string getString(const std::string& key, const std::string& defaultValue);

        /// @brief Defines a new integer setting.
        ///
        /// If the setting already exists, overwrites its value.
        ///
        /// @param key Key.
        /// @param value Value.
        /// @return Current value.
        void setInteger(const std::string& key, int value);

        /// @brief Retrieves the integer setting with the given @p key.
        ///
        /// If no setting exists with such key, sets it to @p defaultValue and returns it.
        /// If the setting exists but its value is not a valid integer, returns @p defaultValue.
        ///
        /// @param key Key.
        /// @param defaultValue Default value.
        /// @return Current value.
        int getInteger(const std::string& key, int defaultValue);

        /// @brief Defines a new double setting.
        ///
        /// If the setting already exists, overwrites its value.
        ///
        /// @param key Key.
        /// @param value Value.
        void setDouble(const std::string& key, double value);

        /// @brief Retrieves the double setting with the given @p key.
        ///
        /// If no setting exists with such key, sets it to @p defaultValue and returns it.
        /// If the setting exists but its value is not a valid double, returns the default value.
        ///
        /// @param key Key.
        /// @param defaultValue Default value.
        /// @return Current value.
        double getDouble(const std::string& key, double defaultValue);

        /// @brief Merges the settings from @p settingsToMerge.
        ///
        /// If a setting from @p settingsToMerge is already defined in this instance, its value is
        /// overwritten.
        ///
        /// @param settingsToMerge Settings to be merged to this instance.
        void merge(const Settings& settingsToMerge);

        /// @brief Save settings as json to specified path.
        /// @param path Virtual `cubos::core::data::FileSystem` path to write settings in.
        /// @param indent Indent of the Json, -1 for no whitespace.
        /// @return Whether call was successful.
        bool save(std::string_view path = "/settings.json", int indent = 2) const;

        /// @brief Load settings from json in specified path.
        /// @param path Virtual `cubos::core::data::FileSystem` path to read settings from.
        /// @return Loaded settings object.
        static Settings load(std::string_view path = "/settings.json");

        /// @return Underlying `std::unordered_map` with the settings.
        const std::unordered_map<std::string, std::string>& getValues() const;

        /// @return Underlying `std::unordered_map` with the settings.
        std::unordered_map<std::string, std::string>& getValues();

    private:
        std::unordered_map<std::string, std::string> mValues;
    };
} // namespace cubos::engine
