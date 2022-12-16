#include <cubos/engine/plugins/file_settings.hpp>

#include <cubos/core/data/json_deserializer.hpp>

#include <cubos/core/data/file_system.hpp>

#include <cubos/core/settings.hpp>

using namespace cubos::core::data;

static void startup(const std::string& path, cubos::core::Settings& settings)
{
    std::string contents;
    auto stream = FileSystem::open(path, File::OpenMode::Read);
    stream->readUntil(contents, nullptr);

    auto deserializer = JSONDeserializer(contents);
    deserializer.read(settings);
}

void cubos::engine::plugins::fileSettingsPlugin(Cubos& cubos)
{
    cubos.addStartupSystem(startup, "ReadSettings");
}
