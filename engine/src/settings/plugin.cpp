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

static void mountSettings(std::string_view path)
{
    // If the settings file is not mounted, mount it.
    if (FileSystem::find("/settings.json") == nullptr)
    {
        // Mount the real settings file as `/settings.json`.
        auto archive = std::make_unique<StandardArchive>(path, false /*isDirectory*/, false /*readOnly*/);
        if (!archive->initialized() || !FileSystem::mount("/settings.json", std::move(archive)))
        {
            CUBOS_ERROR("Couldn't mount the settings file at {}", path);
            return;
        }
    }
}

void cubos::engine::settingsPlugin(Cubos& cubos)
{
    cubos.resource<Settings>();

    cubos.startupTag(settingsTag);

    cubos.startupSystem("load Settings from file and arguments")
        .tagged(settingsTag)
        .call([](const Arguments& args, Settings& settings) {
            // First, load settings from the command line arguments.
            Settings argsSettings = loadFromArguments(args);
            settings.merge(argsSettings);

            mountSettings(settings.getString("settings.path", "settings.json"));

            // Then load settings from the file, and override it with the command line arguments.
            Settings fileSettings = Settings::load();
            fileSettings.merge(argsSettings);
            settings.merge(fileSettings);
        });
}
