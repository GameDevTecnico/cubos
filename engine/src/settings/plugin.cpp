#include <nlohmann/json.hpp>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/settings/plugin.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;

CUBOS_DEFINE_TAG(cubos::engine::settingsTag);

using namespace cubos::engine;

static Settings loadFromArguments(const Arguments& args)
{
    Settings settings{};

    for (const auto& arg : args.value)
    {
        auto token = arg.find('=');
        if (token == std::string::npos)
        {
            CUBOS_ERROR("Could not parse argument: missing '=' token");
            break;
        }

        auto settingName = arg.substr(0, token);
        auto settingValue = arg.substr(token + 1);
        if (settingValue.empty())
        {
            CUBOS_ERROR("Setting {} must have value", settingName);
            break;
        }

        settings.setString(settingName, settingValue);
    }

    return settings;
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

static Settings loadFromFile(std::string_view path)
{
    // If the settings file is not mounted, mount it.
    if (FileSystem::find("/settings.json") == nullptr)
    {
        // Mount the real settings file as `/settings.json`.
        if (!FileSystem::mount("/settings.json",
                               std::make_unique<StandardArchive>(path, false /*isDirectory*/, false /*readOnly*/)))
        {
            CUBOS_ERROR("Couldn't mount the settings file at {}", path);
            return {};
        }
    }

    // Read the contents of the file.
    std::string contents;
    auto stream = FileSystem::open("/settings.json", File::OpenMode::Read);
    stream->readUntil(contents, nullptr);

    // If the file is empty, ignore it.
    if (contents.empty())
    {
        return {};
    }

    // Parse it as JSON.
    nlohmann::json json{};
    try
    {
        json = nlohmann::json::parse(contents);
    }
    catch (nlohmann::json::parse_error& e)
    {
        CUBOS_ERROR("Couldn't parse the settings file: {}", e.what());
        return {};
    }

    if (!json.is_object())
    {
        CUBOS_ERROR("Expected root element in settings file to be an object, but got {}", json.type_name());
        return {};
    }

    // Read the settings from the JSON.
    Settings settings{};
    loadFromJSON("", json, settings);
    return settings;
}

void cubos::engine::settingsPlugin(Cubos& cubos)
{
    cubos.resource<Settings>();

    cubos.tag(settingsTag);

    cubos.startupSystem("load Settings from file and arguments")
        .tagged(settingsTag)
        .call([](const Arguments& args, Settings& settings) {
            // First, load settings from the command line arguments.
            Settings argsSettings = loadFromArguments(args);
            settings.merge(argsSettings);

            // Then load settings from the file, and override it with the command line arguments.
            Settings fileSettings = loadFromFile(settings.getString("settings.path", "settings.json"));
            fileSettings.merge(argsSettings);
            settings.merge(fileSettings);
        });
}
