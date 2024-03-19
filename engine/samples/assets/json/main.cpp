#include <vector>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

using cubos::core::data::FileSystem;
using cubos::core::memory::Stream;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

using namespace cubos::engine;

/// [Asset type]
/// A simple serializable type which we will be saving and loading.
struct Strings
{
    CUBOS_REFLECT;
    std::vector<std::string> strings;
};

CUBOS_REFLECT_IMPL(Strings)
{
    return Type::create("Strings").with(FieldsTrait{}.withField("strings", &Strings::strings));
}
/// [Asset type]

int main()
{
    Cubos cubos{};

    cubos.addPlugin(assetsPlugin);

    cubos.startupSystem("configure Assets plugin").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    /// [Register bridge]
    cubos.startupSystem("setup bridge to load .strings files").tagged(assetsBridgeTag).call([](Assets& assets) {
        assets.registerBridge(".strings", std::make_unique<JSONBridge<Strings>>());
    });
    /// [Register bridge]

    /// [Loading the asset]
    static const Asset<Strings> SampleAsset = AnyAsset("6f42ae5a-59d1-5df3-8720-83b8df6dd536");

    cubos.startupSystem("access .strings asset").tagged(assetsTag).call([](Assets& assets) {
        auto read = assets.read(SampleAsset);
        for (const auto& str : read->strings)
        {
            Stream::stdOut.printf("String: {}\n", str);
        }
    });
    /// [Loading the asset]

    cubos.run();
}