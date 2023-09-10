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

#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

using cubos::core::data::Deserializer;
using cubos::core::data::FileSystem;
using cubos::core::data::Serializer;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::memory::Stream;

using namespace cubos::engine;

/// [Asset type]
/// A simple serializable type which we will be saving and loading.
struct MyAsset
{
    int anInteger;
    std::vector<std::string> someStrings;
};
/// [Asset type]

/// [Serialization definition]
template <>
void cubos::core::data::serialize<MyAsset>(Serializer& ser, const MyAsset& obj, const char* name)
{
    ser.beginObject(name);
    ser.write(obj.anInteger, "anInteger");
    ser.write(obj.someStrings, "someStrings");
    ser.endObject();
}

template <>
void cubos::core::data::deserialize<MyAsset>(Deserializer& des, MyAsset& obj)
{
    des.beginObject();
    des.read(obj.anInteger);
    des.read(obj.someStrings);
    des.endObject();
}
/// [Serialization definition]

/// [Setting]
static void configSystem(Write<Settings> settings)
{
    // If we want to save assets, we must set this to false.
    settings->setBool("assets.io.readOnly", false);
    /// [Setting]

    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

/// [Register bridge]
static void bridgeSystem(Write<Assets> assets)
{
    assets->registerBridge(".my", std::make_unique<JSONBridge<MyAsset>>());
}
/// [Register bridge]

/// [Create a new asset]
static void saveAndLoadSystem(Write<Assets> assets)
{
    // Create a new asset (with a random UUID).
    auto handle = assets->create(MyAsset{
        .anInteger = 42,
        .someStrings = {"Hello", "World"},
    });
    /// [Create a new asset]

    /// [Save the asset]
    assets->writeMeta(handle)->set("path", "/assets/sample/asset.my");
    assets->save(handle);
    /// [Save the asset]

    /// [Force reload]
    assets->invalidate(handle);
    /// [Force reload]

    /// [Read the asset]
    // Access the asset - will be loaded automatically.
    auto read = assets->read(handle);
    Stream::stdOut.printf("Integer: {}\n", read->anInteger);
    for (const auto& str : read->someStrings)
    {
        Stream::stdOut.printf("String: {}\n", str);
    }
    /// [Read the asset]

    // Wait for input before exiting.
    Stream::stdOut.print("You can now check the contents of the file!\nPress enter to exit...");
    Stream::stdIn.get();

    // Cleanup the created asset.
    FileSystem::destroy("/assets/sample");
}

/// [Run]
int main()
{
    Cubos cubos{};
    cubos.addPlugin(assetsPlugin);
    cubos.startupSystem(configSystem).tagged("cubos.settings");
    cubos.startupSystem(bridgeSystem).tagged("cubos.assets.bridge");
    cubos.startupSystem(saveAndLoadSystem).tagged("cubos.assets");
    cubos.run();
}
/// [Run]
