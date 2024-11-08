#include <nlohmann/json.hpp>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/settings/settings.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;
using cubos::core::memory::Stream;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Settings)
{
    return core::ecs::TypeBuilder<Settings>("cubos::engine::Settings").build();
}

void Settings::clear()
{
    mValues.clear();
}

void Settings::setBool(const std::string& key, bool value)
{
    mValues[key] = value ? "true" : "false";
}

bool Settings::getBool(const std::string& key, bool defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, defaultValue ? "true" : "false"});
    }

    return mValues.at(key) == "true";
}

void Settings::setString(const std::string& key, const std::string& value)
{
    mValues[key] = value;
}

std::string Settings::getString(const std::string& key, const std::string& defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, defaultValue});
    }

    return mValues.at(key);
}

void Settings::setInteger(const std::string& key, int value)
{
    mValues[key] = std::to_string(value);
}

int Settings::getInteger(const std::string& key, int defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, std::to_string(defaultValue)});
    }

    try
    {
        return std::stoi(mValues.at(key));
    }
    catch (...)
    {
        mValues.insert({key, std::to_string(defaultValue)});
        return defaultValue;
    }
}

void Settings::setDouble(const std::string& key, double value)
{
    mValues[key] = std::to_string(value);
}

double Settings::getDouble(const std::string& key, double defaultValue)
{
    if (!mValues.contains(key))
    {
        mValues.insert({key, std::to_string(defaultValue)});
    }

    try
    {
        return std::stod(mValues.at(key));
    }
    catch (...)
    {
        return defaultValue;
    }
}

void Settings::merge(const Settings& settingsToMerge)
{
    for (const auto& value : settingsToMerge.mValues)
    {
        this->setString(value.first, value.second);
    }
}

static inline void saveSettingsAsJson(const Settings& settings, Stream& stream, int indent)
{
    auto json = nlohmann::json();
    for (const auto& value : settings.getValues())
    {
        char* pEnd;
        double res = strtod(value.second.c_str(), &pEnd);
        // Store as string if second value is not number nor boolean
        if (pEnd == value.second.c_str() + value.second.length() && !value.second.empty())
        {
            json[value.first] = res;
        }
        else if (value.second == "true" || value.second == "false")
        {
            json[value.first] = value.second == "true";
        }
        else
        {
            json[value.first] = value.second;
        }
    }
    stream.print(json.dump(indent));
}

bool Settings::save(std::string_view path, int indent) const
{
    // If the settings file is not mounted.
    if (FileSystem::find(path) == nullptr)
    {
        return false;
    }

    auto stream = FileSystem::open(path, File::OpenMode::Write);
    saveSettingsAsJson(*this, *stream, indent);
    return true;
}

static void loadFromJSON(const std::string& prefix, const nlohmann::json& json, Settings& settings)
{
    for (auto it = json.begin(); it != json.end(); ++it)
    {
        const auto& key = it.key();
        const auto& value = it.value();

        if (value.is_object())
        {
            loadFromJSON(prefix + key + ".", value, settings);
        }
        else if (value.is_boolean())
        {
            settings.setBool(prefix + key, value);
        }
        else if (value.is_string())
        {
            settings.setString(prefix + key, value);
        }
        else if (value.is_number_integer())
        {
            settings.setInteger(prefix + key, value);
        }
        else if (value.is_number_float())
        {
            settings.setDouble(prefix + key, value);
        }
        else
        {
            CUBOS_ERROR("Unsupported type {} for setting {}", value.type_name(), prefix + key);
        }
    }
}

const std::unordered_map<std::string, std::string>& Settings::getValues() const
{
    return mValues;
}

std::unordered_map<std::string, std::string>& Settings::getValues()
{
    return mValues;
}
