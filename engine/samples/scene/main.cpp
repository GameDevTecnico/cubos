#include <cubos/core/log.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/data/debug_serializer.hpp>

#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/scene.hpp>

#include <cubos/engine/plugins/window.hpp>
#include <cubos/engine/plugins/env_settings.hpp>

#include <components/num.hpp>
#include <components/parent.hpp>

using namespace cubos;
using namespace engine;
using namespace core::ecs;
using namespace core::data;
using namespace core::memory;

void setup(CommandBuffer& cmds, data::AssetManager& assetManager)
{
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    assetManager.registerType<data::Scene>();
    assetManager.importMeta(FileSystem::find("/assets/"));

    // Load and spawn the scene.
    auto scene = assetManager.load<data::Scene>("scenes/main");
    auto root = cmds.create().entity();
    cmds.spawn(scene->blueprint).add("main", Parent{root});
}

void printStuff(Debug debug)
{
    for (auto [entity, pkg] : debug)
    {
        auto name = std::to_string(entity.index);
        auto ser = DebugSerializer(Stream::stdOut);
        ser.write(pkg, name.c_str());
        Stream::stdOut.put('\n');
    }
}

int main(int argc, char** argv)
{
    // Initialize the asset manager.
    Cubos(argc, argv)
        .addResource<data::AssetManager>()
        .addComponent<Num>()
        .addComponent<Parent>()

        .addStartupSystem(setup, "Setup")
        .addStartupSystem(printStuff, "End")

        .addPlugin(cubos::engine::plugins::envSettingsPlugin)
        .addPlugin(cubos::engine::plugins::windowPlugin)
        .run();
}
