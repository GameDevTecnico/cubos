#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/assets/bridges/old/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

using cubos::core::data::FileSystem;
using cubos::core::data::old::Deserializer;
using cubos::core::data::old::Serializer;
using cubos::core::memory::Stream;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

using namespace cubos::engine;

/// [Asset type]
struct IntegerAsset
{
    CUBOS_REFLECT;
    int value;
};

CUBOS_REFLECT_IMPL(IntegerAsset)
{
    return Type::create("IntegerAsset").with(FieldsTrait{}.withField("value", &IntegerAsset::value));
}
/// [Asset type]

template <>
void cubos::core::data::old::serialize<IntegerAsset>(Serializer& ser, const IntegerAsset& obj, const char* name)
{
    ser.beginObject(name);
    ser.write(obj.value, "value");
    ser.endObject();
}

template <>
void cubos::core::data::old::deserialize<IntegerAsset>(Deserializer& des, IntegerAsset& obj)
{
    des.beginObject();
    des.read(obj.value);
    des.endObject();
}

/// [Setting]
static void configSystem(Settings& settings)
{
    // If we want to save assets, we must set this to false.
    settings.setBool("assets.io.readOnly", false);
    /// [Setting]

    settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void bridgeSystem(Assets& assets)
{
    assets.registerBridge(".int", std::make_unique<old::JSONBridge<IntegerAsset>>());
}

/// [Create a new asset]
static void saveSystem(Assets& assets)
{
    // Create a new asset (with a random UUID).
    auto handle = assets.create(IntegerAsset{1337});
    /// [Create a new asset]

    /// [Save the asset]
    assets.writeMeta(handle)->set("path", "/assets/sample/sample.int");
    assets.save(handle);
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
