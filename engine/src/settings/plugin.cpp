#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>
#include <cubos/core/data/old/json_deserializer.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/settings/plugin.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;
using cubos::core::data::old::JSONDeserializer;

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

static Settings loadFromFile(std::string_view path)
{
    // Mount the real settings file as `/settings.json`.
    FileSystem::mount("/settings.json",
                      std::make_unique<StandardArchive>(path, false /*isDirectory*/, false /*readOnly*/));

    // Read the contents of the file.
    std::string contents;
    auto stream = FileSystem::open("/settings.json", File::OpenMode::Read);
    stream->readUntil(contents, nullptr);

    // Parse it as JSON.
    Settings settings{};
    if (!contents.empty())
    {
        JSONDeserializer deserializer{contents};
        deserializer.read(settings);
        if (deserializer.failed())
        {
        }
    }

    return settings;
}

void cubos::engine::settingsPlugin(Cubos& cubos)
{
    cubos.addResource<Settings>();

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
