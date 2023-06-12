/// @file
/// @brief A sample demonstrating how to save and load JSON assets.
///
/// This sample demonstrates how we can use the JSONBridge to easily save and load serializable
/// assets as JSON files. If we had a serializable asset and we wanted to store it as binary, we
/// could simply replace the JSONBridge with a BinaryBridge and the sample would work the same way.
///
/// In this sample, we will be creating an asset of type MyAsset, saving it to a JSON file, and
/// then loading it back from the file.

#include <vector>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>

using cubos::core::Settings;
using cubos::core::data::Deserializer;
using cubos::core::data::FileSystem;
using cubos::core::data::Serializer;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::memory::Stream;
using namespace cubos::engine;

/// A simple serializable type which we will be saving and loading.
struct MyAsset
{
    int an_integer;
    std::vector<std::string> some_strings;
};

// We must implement the cubos::core::data::serialize function for our type, so that it can be
// serialized by the JSONBridge.
template <>
void cubos::core::data::serialize<MyAsset>(Serializer& ser, const MyAsset& obj, const char* name)
{
    ser.beginObject(name);
    ser.write(obj.an_integer, "an_integer");
    ser.write(obj.some_strings, "some_strings");
    ser.endObject();
}

// We must also implement the cubos::core::data::deserialize function for our type, so that it can
// be deserialized by the JSONBridge.
template <>
void cubos::core::data::deserialize<MyAsset>(Deserializer& des, MyAsset& obj)
{
    des.beginObject();
    des.read(obj.an_integer);
    des.read(obj.some_strings);
    des.endObject();
}

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);

    // If we want to save assets, we must set this to false.
    settings->setBool("assets.io.readOnly", false);
}

static void bridge(Write<Assets> assets)
{
    // Add the a JSONBridge for the .my extension.
    assets->registerBridge(".my", std::make_unique<JSONBridge<MyAsset>>());
}

static void save_and_load(Write<Assets> assets)
{
    // Create a new asset (with a random UUID).
    auto handle = assets->create(MyAsset{
        .an_integer = 42,
        .some_strings = {"Hello", "World"},
    });

    // Set a path for the asset - necessary for the JSONBridge to know where to save the asset.
    // It is important to set the correct extension, so that the asset manager knows which bridge
    // to use.
    assets->writeMeta(handle)->set("path", "/assets/sample/asset.my");

    // Save the asset.
    assets->save(handle);

    // After making the handle weak, the asset won't have any more strong references.
    handle.makeWeak();

    // By calling cleanup, we can remove the asset from memory, forcing it to be reloaded from disk
    // the next time it is accessed.
    assets->cleanup();

    // Access the asset - will be loaded automatically.
    auto read = assets->read(handle);
    Stream::stdOut.printf("Integer: {}\n", read->an_integer);
    for (const auto& str : read->some_strings)
    {
        Stream::stdOut.printf("String: {}\n", str);
    }

    // Wait for input before exiting.
    Stream::stdOut.print("You can now check the contents of the file!\nPress enter to exit...");
    Stream::stdIn.get();

    // Cleanup the created asset.
    FileSystem::destroy("/assets/sample");
}

int main()
{
    auto cubos = Cubos();
    cubos.addPlugin(assetsPlugin);
    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
    cubos.startupSystem(save_and_load).tagged("cubos.assets");
    cubos.run();
    return 0;
}
