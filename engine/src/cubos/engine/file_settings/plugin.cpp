#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/json_deserializer.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/file_settings/plugin.hpp>

using namespace cubos::core::data;

static void startup(cubos::core::Settings& settings)
{
    // Get the path of the real settings file, mount an archive on it.
    auto path = settings.getString("settings.path", "settings.json");
    FileSystem::mount("/settings.json", std::make_shared<STDArchive>(path, false, false));

    std::string contents;
    auto stream = FileSystem::open("/settings.json", File::OpenMode::Read);
    stream->readUntil(contents, nullptr);
    if (!contents.empty())
    {
        cubos::core::Settings newSettings;
        auto deserializer = JSONDeserializer(contents);
        deserializer.read(newSettings);
        settings.merge(newSettings);
    }
}

void cubos::engine::fileSettingsPlugin(Cubos& cubos)
{
    cubos.startupSystem(startup).tagged("cubos.settings").tagged("cubos.settings.file");
}
