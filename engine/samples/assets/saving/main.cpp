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
struct IntegerAsset
{
    int value;
};
/// [Asset type]

template <>
void cubos::core::data::serialize<IntegerAsset>(Serializer& ser, const IntegerAsset& obj, const char* name)
{
    ser.beginObject(name);
    ser.write(obj.value, "value");
    ser.endObject();
}

template <>
void cubos::core::data::deserialize<IntegerAsset>(Deserializer& des, IntegerAsset& obj)
{
    des.beginObject();
    des.read(obj.value);
    des.endObject();
}

/// [Setting]
static void configSystem(Write<Settings> settings)
{
    // If we want to save assets, we must set this to false.
    settings->setBool("assets.io.readOnly", false);
    /// [Setting]

    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void bridgeSystem(Write<Assets> assets)
{
    assets->registerBridge(".int", std::make_unique<JSONBridge<IntegerAsset>>());
}

/// [Create a new asset]
static void saveSystem(Write<Assets> assets)
{
    // Create a new asset (with a random UUID).
    auto handle = assets->create(IntegerAsset{1337});
    /// [Create a new asset]

    /// [Save the asset]
    assets->writeMeta(handle)->set("path", "/assets/sample/sample.int");
    assets->save(handle);
    /// [Save the asset]

    // Wait for input before exiting.
    Stream::stdOut.print("You can now check the contents of the file!\nPress enter to exit...");
    Stream::stdIn.get();

    // Cleanup the created asset.
    FileSystem::destroy("/assets/sample");
}

int main()
{
    Cubos cubos{};

    /// [Configure]
    cubos.addPlugin(assetsPlugin);
    cubos.startupSystem(saveSystem).tagged("cubos.assets");
    /// [Configure]

    cubos.startupSystem(configSystem).tagged("cubos.settings");
    cubos.startupSystem(bridgeSystem).tagged("cubos.assets.bridge");
    cubos.run();
}
