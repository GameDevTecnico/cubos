#include <doctest/doctest.h>

#include <cubos/core/data/fs/embedded_archive.hpp>
#include <cubos/core/data/fs/file_system.hpp>

#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::core::data;
using namespace cubos::engine;

TEST_CASE("cubos::engine::settingsPlugin")
{
    // Mock command line arguments.
    int argc = 4;
    char* argv[] = {
        const_cast<char*>("cubos"),
        const_cast<char*>("my.boolean.setting=true"),
        const_cast<char*>("my.string.setting=hello"),
        const_cast<char*>("my.int.setting=42"),
        nullptr,
    };

    // Mock settings file.
    const char settingsJSON[] = R"(
        {
            "my": {
                "boolean": {
                    "setting": false,
                    "setting2": true
                },
                "string.setting": "world",
                "string.setting2": "world"
            },
            "my.int.setting": 13,
            "my.int.setting2": 9
        }
    )";

    const EmbeddedArchive::Data::Entry settingsEntry = {
        .name = "settings.json",
        .isDirectory = false,
        .parent = 0,
        .sibling = 0,
        .child = 0,
        .data = settingsJSON,
        .size = sizeof(settingsJSON),
    };

    const EmbeddedArchive::Data settingsData = {
        .entries = &settingsEntry,
        .entryCount = 1,
    };

    EmbeddedArchive::registerData("settings.json", settingsData);
    FileSystem::mount("/settings.json", std::make_unique<EmbeddedArchive>("settings.json"));

    // Run the actual test.
    Cubos cubos{argc, argv};
    cubos.plugin(settingsPlugin);

    cubos.startupSystem("add some settings before load").before(settingsTag).call([](Settings& settings) {
        settings.setInteger("my.int.setting", 5);
        settings.setString("kept.string", "bye");
    });

    cubos.startupSystem("check settings").after(settingsTag).call([](Settings& settings) {
        // File settings are overridden by command line arguments.
        CHECK(settings.getBool("my.boolean.setting", false));
        CHECK(settings.getString("my.string.setting", "bye") == "hello");
        CHECK(settings.getInteger("my.int.setting", 7) == 42);
        CHECK(settings.getString("kept.string", "hello") == "bye");

        // File settings not set in the command line arguments are kept.
        CHECK(settings.getBool("my.boolean.setting2", false));
        CHECK(settings.getString("my.string.setting2", "hello") == "world");
        CHECK(settings.getInteger("my.int.setting2", 7) == 9);

        // Settings not set in both the file and the command line arguments are kept.
        CHECK(settings.getString("kept.string", "hello") == "bye");
    });

    cubos.run();
}
