#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/assets/bridges/old/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

using cubos::core::data::FileSystem;
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

int main()
{
    Cubos cubos{};

    cubos.addPlugin(assetsPlugin);

    /// [Setting]
    cubos.startupSystem("configure Assets plugin").tagged("cubos.settings").call([](Settings& settings) {
        // If we want to save assets, we must set this to false.
        settings.setBool("assets.io.readOnly", false);
        /// [Setting]

        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("setup bridge to load and save .int assets")
        .tagged("cubos.assets.bridge")
        .call([](Assets& assets) { assets.registerBridge(".int", std::make_unique<JSONBridge<IntegerAsset>>()); });

    /// [Create a new asset]
    cubos.startupSystem("create and save asset").tagged("cubos.assets").call([](Assets& assets) {
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
    });

    cubos.run();
}
